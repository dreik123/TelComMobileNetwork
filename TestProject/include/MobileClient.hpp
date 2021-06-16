#ifndef _MOBILE_CLIENT_HPP_
#define _MOBILE_CLIENT_HPP_
#include "NetConfAgent.hpp"
class MobileClient {
public:
    MobileClient();
    bool registerClient(const std::string& number);
    void setName(const std::string& name);
    void handleModuleChange();
    void handleOperData();
private:
    std::shared_ptr<NetConfAgent> _agent;
    std::string _userName;
    std::string _number;
};
#endif