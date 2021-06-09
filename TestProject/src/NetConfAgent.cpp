#include "NetConfAgent.hpp"
#include <iostream>
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
    char select_xpath[100];
    try {
        snprintf(select_xpath, 100, "/%s:*//*", module_name);

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

bool NetConfAgent::subscribeForModelChanges(const std::string& module)
{
    auto cb = [] (sysrepo::S_Session sess, const char *module_name, const char *xpath, sr_event_t event,
            uint32_t request_id) {
            char change_path[100];

            try {
                cout << "\n\n ========== Notification " << ev_to_str(event) << " =============================================";
                if (SR_EV_CHANGE == event) {
                    cout << "\n\n ========== CONFIG HAS CHANGED, CURRENT RUNNING CONFIG: ==========\n" << endl;
                    print_current_config(sess, module_name);
                }

                cout << "\n\n ========== CHANGES: =============================================\n" << endl;

                snprintf(change_path, 100, "/%s:*//.", module_name);

                auto it = sess->get_changes_iter(change_path);

                while (auto change = sess->get_change_next(it)) {
                    print_change(change);
                }

                cout << "\n\n ========== END OF CHANGES =======================================\n" << endl;

            } catch( const std::exception& e ) {
                cout << e.what() << endl;
            }
            return SR_ERR_OK;
        };   
    auto subscribe = std::make_shared<sysrepo::Subscribe>(std::move(sess)); 
    subscribe->module_change_subscribe(module.c_str(), cb);
    cout << "subscribed" << endl;
    return true;
}

bool NetConfAgent::registerOperData(const std::string& module, const std::string& xpath)
{
            auto cb = [] (sysrepo::S_Session session, const char *module_name, const char *path, const char *request_xpath,
            uint32_t request_id, libyang::S_Data_Node &parent) {

            cout << "\n\n ========== CALLBACK CALLED TO PROVIDE \"" << path << "\" DATA ==========\n" << endl;
            cout << "module_name: " << module_name << endl;
            cout << "path: " << path << endl;
            cout << "request_xpath: " << request_xpath << endl;
            libyang::S_Context ctx = session->get_context();
            libyang::S_Module mod = ctx->get_module(module_name);
            auto subscribers = std::make_shared<libyang::Data_Node>(parent, mod, "subscribers");
            auto userName = std::make_shared<libyang::Data_Node>(subscribers, mod, "userName");
            return SR_ERR_OK;
        };
        
    auto subscribe = std::make_shared<sysrepo::Subscribe>(std::move(sess)); 
    std::cout << "register" << std::endl;
    subscribe->oper_get_items_subscribe(module.c_str(), cb, xpath.c_str());
    return true;
}


bool NetConfAgent::subscribeForRpc(const std::string& xpath)
{
    std::cout << "Subscribe for rpc" << std::endl;
    auto cbVals = [](sysrepo::S_Session session, const char* op_path, const sysrepo::S_Vals input, sr_event_t event, uint32_t request_id, sysrepo::S_Vals_Holder output) {
        std::cout << "\n ========== RPC CALLED ==========\n" << std::endl;
        return SR_ERR_OK;
    };
    auto subscribe = std::make_shared<sysrepo::Subscribe>(std::move(sess)); 
    subscribe->rpc_subscribe(xpath.c_str(), cbVals, 1);
    return true;
}

bool NetConfAgent::notifySysrepo(const std::string& module)
{
    auto cbVals = [] (sysrepo::S_Session session, const sr_ev_notif_type_t notif_type, const char *path,
        const sysrepo::S_Vals vals, time_t timestamp) {
        cout << "\n ========== NOTIF RECEIVED ==========\n" << endl;
    };
    auto subscribe = std::make_shared<sysrepo::Subscribe>(std::move(sess)); 
    subscribe->event_notif_subscribe(module.c_str(), cbVals);
    return true;
}

bool NetConfAgent::changeData(const std::string& xpath, std::string value)
{
    sess->set_item_str(xpath.c_str(), value.c_str());
    sess->apply_changes();
    return true;
}