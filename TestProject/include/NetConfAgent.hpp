#ifndef _NET_CONF_AGENT_HPP_
#define _NET_CONF_AGENT_HPP_
#include <memory>
#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/Connection.hpp>
#include <string>
#include <vector>
#include <map>

/**
 * \brief Class for calling sysrepo functions
 * \author Andrii Pukhalevych
 * \version 1.0
 * A class that mediates between client and sysrepo functions
 * */
class NetConfAgent {
public:
    /**
     * Initializes private fields of the class
     * */
    bool initSysrepo();
    /**
     * Gets the requested data from the model
     * \param[in] xpath Path to the requested data
     * \param[out] res The container with the received data. The key is xpath, the value is the value of the attribute
     * */
    bool fetchData(const std::string& xpath, std::map<std::string, std::string>& res);
    /**
     * Subscribing for changes in the model and calling the function if they occur
     * \param[in] module The name of the model we are subscribing to changes
     * */
    bool subscribeForModelChanges(const std::string& module);
    /**
     * Register for providing operational data 
     * \param[in] module The name of the model for providing operational data
     * \param[in] xpath Path to the operational data
     * \param[in] data Container with operational data. The key is xpath, the value is the value of the attribute
     * */
    bool registerOperData(const std::string& module, const std::string& xpath, const std::map<std::string, std::string>& data);
    /**
     * Subscribes to receive rpc from the user and send feedback to the user
     * \param[in] xpath Path to rpc
     * \param[in] output_val_for_rpc Data to send user feedback from rpc
     * */
    bool subscribeForRpc(const std::string& xpath, const std::map<std::string, std::string>& output_val_for_rpc);
    /**
     * Sends a notification to the user
     * \param[in] xpath Path to notification
     * \param[in] values Container for notifications. The key is xpath, the value is the value of the attribute
     * */
    bool notifySysrepo(const std::string& xpath, const std::map<std::string, std::string>& values);
    /**
     * Modifies or creates the requested data
     * \param[in] xpath Path to change data
     * \param[in] values Value to change data
     * */
    bool changeData(const std::string& xpath, const std::string& value);
private:
    std::shared_ptr<sysrepo::Session> sess; /**< sysrepo session */
    std::shared_ptr<sysrepo::Connection> conn; /**< sysrepo connection */
    std::shared_ptr<sysrepo::Subscribe> subscribe; /**< sysrepo subscription */
};
#endif