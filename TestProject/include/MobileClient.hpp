#ifndef _MOBILE_CLIENT_HPP_
#define _MOBILE_CLIENT_HPP_
#include "NetConfAgent.hpp"
class MobileClient {
public:
    MobileClient();
    bool registerClient(const std::string& number);
    bool isRegister();
    bool isCall();
    void setName(const std::string& name);
    void call(const std::string& number);
    void answer();
    void reject();
    void endCall();
    void unregister();
    void unregister(const std::string& number);
    void handleModuleChange(sysrepo::S_Change change);
    void handleOperData(libyang::S_Context& ctx, libyang::S_Data_Node &parent, const std::map<std::string, std::string>& data);
private:
    std::shared_ptr<NetConfAgent> _agent;
    std::string _userName;
    std::string _number;
    bool _register;
    bool _call;
    std::string _abbonentB;
};
#endif