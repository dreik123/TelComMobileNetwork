#include "NetConfAgent.hpp"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

using std::cout;
using std::endl;
bool NetConfAgent::initSysrepo()
{
    conn = std::make_shared<sysrepo::Connection>();
    sess = std::make_shared<sysrepo::Session>(conn);
    subscribe = std::make_shared<sysrepo::Subscribe>(sess);
    return true;
}

bool NetConfAgent::fetchData(const std::string& xpath, std::map<std::string, std::string>& res)
{
    auto values = sess->get_item(xpath.c_str());
    if (values == nullptr)
        return false;

    res.insert(std::pair<std::string, std::string>(std::string(values->xpath()), 
    std::string(values->data()->get_string())));
            
    return true;
}
namespace 
{
    void print_change(sysrepo::S_Change change) 
    {
    cout << endl;
    switch(change->oper()) {
    case SR_OP_CREATED:
        if (nullptr != change->new_val()) {
           cout <<"CREATED: ";
           cout << change->new_val()->to_string();
        }
        break;
    case SR_OP_DELETED:
        if (nullptr != change->old_val()) {
           cout << "DELETED: ";
           cout << change->old_val()->to_string();
        }
    break;
    case SR_OP_MODIFIED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) {
           cout << "MODIFIED: ";
           cout << "old value ";
           cout << change->old_val()->to_string();
           cout << "new value ";
           cout << change->new_val()->to_string();
        }
    break;
    case SR_OP_MOVED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) {
           cout << "MOVED: ";
           cout << change->new_val()->xpath();
           cout << " after ";
           cout << change->old_val()->xpath();
        } else if (nullptr != change->new_val()) {
           cout << "MOVED: ";
           cout << change->new_val()->xpath();
           cout << " first";
        }
    break;
    }
}
}


bool NetConfAgent::subscribeForModelChanges(const std::string& module)
{
    auto cb = [] (sysrepo::S_Session sess, const char *module_name, const char *xpath, sr_event_t event,
    uint32_t request_id) 
    {
        std::string change_path(module_name);
        auto it = sess->get_changes_iter(change_path.c_str());

        while (auto change = sess->get_change_next(it)) 
        {
            print_change(change);
        }
        return SR_ERR_OK;
    };    
    subscribe->module_change_subscribe(module.c_str(), cb);
    return true;
}

bool NetConfAgent::registerOperData(const std::string& module, const std::string& xpath, const std::map<std::string, std::string>& data)
{
    auto cb = [data] (sysrepo::S_Session session, const char *module_name, const char *path, const char *request_xpath,
    uint32_t request_id, libyang::S_Data_Node &parent) 
    {
        libyang::S_Context ctx = session->get_context();
        libyang::S_Module mod = ctx->get_module(module_name);
        for(auto &d: data)
        {
            parent->new_path(ctx, d.first.c_str(), d.second.c_str(), LYD_ANYDATA_CONSTSTRING, 0);
        }
        
        /*auto subscribers = std::make_shared<libyang::Data_Node>(parent, mod, "subscribers");
        auto number = std::make_shared<libyang::Data_Node>(subscribers, mod, "number", "+380877676678");
        auto userName = std::make_shared<libyang::Data_Node>(subscribers, mod, "userName", "bob");*/
        return SR_ERR_OK;
    };

    subscribe->oper_get_items_subscribe(module.c_str(), cb, xpath.c_str());

    return true;
}


bool NetConfAgent::subscribeForRpc(const std::string& xpath, const std::map<std::string, std::string>& output_val_for_rpc)
{
    auto cbVals = [output_val_for_rpc](sysrepo::S_Session session, const char* op_path, const sysrepo::S_Vals input, 
    sr_event_t event, uint32_t request_id, sysrepo::S_Vals_Holder output) 
    {
        auto out_vals = output->allocate(output_val_for_rpc.size());
        int index = 0;
        for(auto& v: output_val_for_rpc)
        {
            out_vals->val(index)->set(v.first.c_str(), v.second.c_str(), SR_STRING_T);
            ++index;
        }
        return SR_ERR_OK;
    };
    subscribe->rpc_subscribe(xpath.c_str(), cbVals, 1);
    return true;
}

bool NetConfAgent::notifySysrepo(const std::string& xpath, const std::map<std::string, std::string>& values)
{
    auto in_vals = std::make_shared<sysrepo::Vals>(values.size());
    int index = 0;
    for(auto& v: values)
    {
        in_vals->val(index)->set(v.first.c_str(), v.second.c_str(), SR_STRING_T);
        ++index;
    }
    sess->event_notif_send(xpath.c_str(), in_vals);
    return true;
}

bool NetConfAgent::changeData(const std::string& xpath, const std::string& value)
{
    sess->set_item_str(xpath.c_str(), value.c_str());
    sess->apply_changes();
    return true;
}