#ifndef _MOBILE_CLIENT_HPP_
#define _MOBILE_CLIENT_HPP_
#include "NetConfAgent.hpp"
/**
 * \brief Class for mediation between client and NetConfAgent
 * \author Andrii Pukhalevych
 * \version 1.0
 * A class for handling client commands and for mediation between client and NetConfAgent
 * */
class MobileClient {
public:
    /**
     * Constructor initializes private fields of the class
     * */
    MobileClient();
    MobileClient(std::unique_ptr<NetConfAgent> agent);
    /**
     * Register client number in sysrepo
     * @param number number for register
     * */
    bool registerClient(const std::string& number);
    /**
     * Checks if client already registered in application
     * */
    bool isRegister();
    /**
     * Checks if client state active or busy (in call)
     * */
    bool isCall();
    /**
     * Sets name client
     * @param name name client
     * */
    void setName(const std::string& name);
    /**
     * Command call another client
     * @param number number another client
     * */
    void call(const std::string& number);
    /**
     * Answer on call
     * */
    void answer();
    /**
     * Reject on call
     * */
    void reject();
    /**
     * End current call
     * */
    void endCall();
    /**
     * Unregister current client
     * */
    void unregister();
    //void unregister(const std::string& number);
    /**
     * Method called from NetConfAgent class on model changes
     * @param change changes in model
     * */
    void handleModuleChange(sysrepo::S_Change change);
    /**
     * Method called from NetConfAgent class on fetch operational data
     * */
    void handleOperData(libyang::S_Context& ctx, libyang::S_Data_Node &parent, const std::map<std::string, std::string>& data);
private:
    std::unique_ptr<NetConfAgent> _agent; /**< field for connection with NetConfAgent class */
    std::string _userName; /**< name client */
    std::string _number; /**< number client */
    bool _register; /**< is register client */
    bool _call; /**< is call client */
    std::string _abbonentB; /**< number another client, when we are calling him */
};
#endif