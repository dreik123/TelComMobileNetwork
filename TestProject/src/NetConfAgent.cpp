#include "NetConfAgent.hpp"
#include <iostream>
using std::cout;
using std::endl;
bool NetConfAgent::initSysrepo()
{
    conn = std::make_unique<sysrepo::Connection>();
    sess = std::make_unique<sysrepo::Session>(std::move(conn));
    subscribe = std::make_unique<sysrepo::Subscribe>(std::move(sess));
    return true;
}

bool NetConfAgent::closeSysrepo()
{
    std::cout << "session stop" << std::endl;
    sess->session_stop();
    return true;
}

bool NetConfAgent::fetchData(const std::string& xpath)
{
    libyang::S_Data_Node data = sess->get_data(xpath.c_str());

        /* go through all top-level siblings */
    for (libyang::S_Data_Node &root : data->tree_for()) {
        /* go through all the children of a top-level sibling */
        for (libyang::S_Data_Node &node : root->tree_dfs()) {
            std::cout << node->path() << std::endl;
        }
    }
    return true;
}

bool NetConfAgent::subscribeForModelChanges(const std::string& module)
{
    auto cb = [] (sysrepo::S_Session sess, const char *module_name, const char *xpath, sr_event_t event,
            uint32_t request_id) {
            std::cout << "\n ========== SUBSCRIBE FOR MODULE CHANGES ==========\n" << std::endl;
            return SR_ERR_OK;
        };
    subscribe->module_change_subscribe(module.c_str(), cb, nullptr, 0, SR_SUBSCR_DONE_ONLY);
    return true;
}

bool NetConfAgent::registerOperData(const std::string& module, const std::string& xpath)
{
            auto cb = [] (sysrepo::S_Session session, const char *module_name, const char *path, const char *request_xpath,
            uint32_t request_id, libyang::S_Data_Node &parent) {

            cout << "\n\n ========== CALLBACK CALLED TO PROVIDE \"" << path << "\" DATA ==========\n" << endl;

            return SR_ERR_OK;
        };
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
    subscribe->rpc_subscribe(xpath.c_str(), cbVals, 1);
    return true;
}

bool NetConfAgent::notifySysrepo(const std::string& module)
{
    auto cbVals = [] (sysrepo::S_Session session, const sr_ev_notif_type_t notif_type, const char *path,
        const sysrepo::S_Vals vals, time_t timestamp) {
        cout << "\n ========== NOTIF RECEIVED ==========\n" << endl;
    };
    subscribe->event_notif_subscribe(module.c_str(), cbVals);
    return true;
}

bool NetConfAgent::changeData(const std::string& xpath, std::map<std::string, std::string> values)
{
    auto input = std::make_shared<sysrepo::Vals>(values.size());
    int count  = 0;
    for(auto &c : values)
    {
        std::string fullXpath = xpath + "/" + c.first;
        std::cout << fullXpath << std::endl;
        input->val(count)->set(fullXpath.c_str(), c.second.c_str());
        ++count;
    }
    sess->rpc_send(xpath.c_str(), input);
    return true;
}