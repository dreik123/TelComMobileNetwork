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
    conn = std::make_unique<sysrepo::Connection>();
    sess = std::make_unique<sysrepo::Session>(std::move(conn));
    return true;
}

bool NetConfAgent::fetchData(const std::string& xpath, std::map<std::string, std::string>& res)
{
    auto values = sess->get_items(xpath.c_str());
           // get_item / get data
    if (values == nullptr)
        return false;

    for(unsigned int i = 0; i < values->val_cnt(); i++)
    {
        res.insert(std::pair<std::string, std::string>(std::string(values->val(i)->xpath()), 
        std::string(values->val(i)->data()->get_string())));
    }
            
    return true;
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

volatile int exit_application = 0;
static void
sigint_handler(int signum)
{
    exit_application = 1;
}
bool NetConfAgent::subscribeForModelChanges(const std::string& module)
{
    auto cb = [] (sysrepo::S_Session sess, const char *module_name, const char *xpath, sr_event_t event,
            uint32_t request_id) {
            cout << "\n\n ========== Notification " << ev_to_str(event) << " =============================================";
            return SR_ERR_OK;
        };   
    auto subscribe = std::make_shared<sysrepo::Subscribe>(std::move(sess)); 
    subscribe->module_change_subscribe(module.c_str(), cb);
    cout << "subscribed" << endl;
            signal(SIGINT, sigint_handler);
        while (!exit_application) {
            sleep(1000);  /* or do some more useful work... */
        }
        std::cout << "quit" << std::endl;
    return true;
}

bool NetConfAgent::registerOperData(const std::string& module, const std::string& xpath)
{
    auto subscribe = std::make_shared<sysrepo::Subscribe>(std::move(sess));
        auto cb = [] (sysrepo::S_Session session, const char *module_name, const char *path, const char *request_xpath,
        uint32_t request_id, libyang::S_Data_Node &parent) {

        cout << "\n\n ========== CALLBACK CALLED TO PROVIDE \"" << path << "\" DATA ==========\n" << endl;

        libyang::S_Context ctx = session->get_context();
        libyang::S_Module mod = ctx->get_module(module_name);
        auto subscribers = std::make_shared<libyang::Data_Node>(parent, mod, "subscribers");
        auto number = std::make_shared<libyang::Data_Node>(subscribers, mod, "number", "+380877676678");
        auto userName = std::make_shared<libyang::Data_Node>(subscribers, mod, "userName", "bob");
        cout << "registered" << endl;
        return SR_ERR_OK;
    };

    std::cout << "register" << std::endl;
    subscribe->oper_get_items_subscribe(module.c_str(), cb, xpath.c_str());
            /* loop until ctrl-c is pressed / SIGINT is received */
        signal(SIGINT, sigint_handler);
        while (!exit_application) {
            sleep(1000);  /* or do some more useful work... */
        }
        std::cout << "quit" << std::endl;
    return true;
}


bool NetConfAgent::subscribeForRpc(const std::string& xpath, const std::map<std::string, std::string>& output_val)
{
    std::cout << "Subscribe for rpc" << std::endl;
    auto cbVals = [output_val](sysrepo::S_Session session, const char* op_path, const sysrepo::S_Vals input, sr_event_t event, uint32_t request_id, sysrepo::S_Vals_Holder output) {
        std::cout << "\n ========== RPC CALLED ==========\n" << std::endl;
        auto out_vals = output->allocate(output_val.size());
        int index = 0;
        for(auto& v: output_val)
        {
            out_vals->val(index)->set(v.first.c_str(), v.second.c_str(), SR_STRING_T);
            ++index;
        }
        return SR_ERR_OK;
    };
    auto subscribe = std::make_shared<sysrepo::Subscribe>(std::move(sess)); 
    subscribe->rpc_subscribe(xpath.c_str(), cbVals, 1);
        signal(SIGINT, sigint_handler);
        while (!exit_application) {
            sleep(1000);  /* or do some more useful work... */
        }
        std::cout << "quit" << std::endl;
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