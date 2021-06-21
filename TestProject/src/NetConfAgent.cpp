#include "NetConfAgent.hpp"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "MobileClient.hpp"
using std::cout;
using std::endl;
bool NetConfAgent::initSysrepo()
{
    _conn = std::make_shared<sysrepo::Connection>();
    _sess = std::make_shared<sysrepo::Session>(_conn);
    _subscribe = std::make_shared<sysrepo::Subscribe>(_sess);
    return true;
}

bool NetConfAgent::fetchData(const std::string& xpath, std::map<std::string, std::string>& res)
{
    auto values = _sess->get_item(xpath.c_str());
    if (values == nullptr)
        return false;

    res.insert(std::pair<std::string, std::string>(std::string(values->xpath()), 
    std::string(values->val_to_string())));
            
    return true;
}
#define MAX_LEN 100
static void
print_change(sysrepo::S_Change change) {
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

/* Function to print current configuration state.
 * It does so by loading all the items of a session and printing them out. */
static void
print_current_config(sysrepo::S_Session session, const char *module_name)
{
    char select_xpath[MAX_LEN];
    try {
        snprintf(select_xpath, MAX_LEN, "/%s:*//*", module_name);

        auto values = session->get_items(&select_xpath[0]);
        if (values == nullptr)
            return;

        for(unsigned int i = 0; i < values->val_cnt(); i++)
            cout << values->val(i)->to_string();
    } catch( const std::exception& e ) {
        cout << e.what() << endl;
    }
}

/* Helper function for printing events. */
const char *ev_to_str(sr_event_t ev) {
    switch (ev) {
    case SR_EV_CHANGE:
        return "change";
    case SR_EV_DONE:
        return "done";
    case SR_EV_ABORT:
    default:
        return "abort";
    }
}

bool NetConfAgent::subscribeForModelChanges(const std::string& module, const std::string& xpath, MobileClient& client)
{
    auto cb = [&client] (sysrepo::S_Session sess, const char *module_name, const char *xpath, sr_event_t event,
    uint32_t request_id) 
    {
        char change_path[MAX_LEN];
        try
        {
            if (SR_EV_DONE == event)
            {

                cout << "\n\n ========== CHANGES: =============================================\n" << endl;

                snprintf(change_path, MAX_LEN, "/%s:*//.", module_name);

                auto it = sess->get_changes_iter(change_path);

                while (auto change = sess->get_change_next(it)) {
                    print_change(change);
                }
                cout << "\n\n ========== END OF CHANGES =======================================\n" << endl;
            }


                
        }catch( const std::exception& e ) {
                cout << e.what() << endl;
            }
            client.handleModuleChange();
            return SR_ERR_OK;
                
    };    
    _subscribe->module_change_subscribe(module.c_str(), cb, xpath.c_str());
    return true;
}

bool NetConfAgent::registerOperData(const std::string& module, const std::string& xpath, const std::map<std::string, std::string>& data, MobileClient& client)
{
    auto cb = [&client, data] (sysrepo::S_Session session, const char *module_name, const char *path, const char *request_xpath,
    uint32_t request_id, libyang::S_Data_Node &parent) 
    {
        libyang::S_Context ctx = session->get_context();
        libyang::S_Module mod = ctx->get_module(module_name);
        client.handleOperData();
        for(auto &d: data)
        {
            parent->new_path(ctx, d.first.c_str(), d.second.c_str(), LYD_ANYDATA_CONSTSTRING, 0);
        }
        
        /*auto subscribers = std::make_shared<libyang::Data_Node>(parent, mod, "subscribers");
        auto number = std::make_shared<libyang::Data_Node>(subscribers, mod, "number", "+380877676678");
        auto userName = std::make_shared<libyang::Data_Node>(subscribers, mod, "userName", "bob");*/
        return SR_ERR_OK;
    };

    _subscribe->oper_get_items_subscribe(module.c_str(), cb, xpath.c_str());

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
    _subscribe->rpc_subscribe(xpath.c_str(), cbVals, 1);
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
    _sess->event_notif_send(xpath.c_str(), in_vals);
    return true;
}

bool NetConfAgent::changeData(const std::string& xpath, const std::string& value)
{
    _sess->set_item_str(xpath.c_str(), value.c_str());
    _sess->apply_changes();
    return true;
}

bool NetConfAgent::changeData(const std::string& xpath)
{
    _sess->delete_item(xpath.c_str());
    _sess->apply_changes();
    return true;
}