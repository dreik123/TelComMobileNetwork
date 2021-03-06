#include "NetConfAgent.hpp"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "MobileClient.hpp"
using std::cout;
using std::endl;
NetConfAgent::~NetConfAgent()
{

}
bool NetConfAgent::initSysrepo()
{
    try {
        _conn = std::make_shared<sysrepo::Connection>();
        _sess = std::make_shared<sysrepo::Session>(_conn);
        _subscribe = std::make_shared<sysrepo::Subscribe>(_sess);
    }
    catch(const std::exception& e)
    {
        return false;
    }
    return true;
}

bool NetConfAgent::fetchData(const std::string& xpath, std::map<std::string, std::string>& res)
{
    try {
        auto values = _sess->get_item(xpath.c_str());
        if (values == nullptr)
            return false;

        res.insert(std::pair<std::string, std::string>(std::string(values->xpath()), 
        std::string(values->val_to_string())));
    }
    catch(const std::exception& e)
    {
        return false;
    }
    
            
    return true;
}


bool NetConfAgent::subscribeForModelChanges(const std::string& module, const std::string& xpath, MobileClient& client)
{
    auto cb = [&client] (sysrepo::S_Session sess, const char *module_name, const char *xpath, sr_event_t event,
    uint32_t request_id) 
    {
        char change_path[100];
        try
        {
            if (SR_EV_DONE == event)
            {
                snprintf(change_path, 100, "/%s:*//.", module_name);
                auto it = sess->get_changes_iter(change_path);
                while (auto change = sess->get_change_next(it)) 
                {
                    if (nullptr != change->new_val()) 
                    {
                        std::string xpath = change->new_val()->to_string();
                        std::string newVal("");
                        std::string oldVal("");
                        if(xpath.find("state") != -1 && nullptr != change->old_val())
                        {
                            newVal = change->new_val()->data()->get_enum();
                            oldVal = change->old_val()->data()->get_enum();
                        }
                        else if (xpath.find("incomingNumber") != -1) 
                        {
                            newVal = change->new_val()->data()->get_string();
                        }


                        client.handleModuleChange(xpath, newVal, oldVal);
                    }
                }
            }
        } 
        catch(const std::exception& e) 
        {
            cout << e.what() << endl;
        }
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
        client.handleOperData(ctx, parent, data);
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

/*bool NetConfAgent::deleteData(const std::string& xpath)
{
    _sess->delete_item(xpath.c_str());
    _sess->apply_changes();
    return true;
}*/

bool NetConfAgent::deleteData(const std::string& xpath, bool isUnregister)
{
    if(isUnregister)
    {
        _conn = std::make_shared<sysrepo::Connection>();
        _sess = std::make_shared<sysrepo::Session>(_conn);
        _subscribe = std::make_shared<sysrepo::Subscribe>(_sess);
    }
    _sess->delete_item(xpath.c_str());
    _sess->apply_changes();
    return true;
}

