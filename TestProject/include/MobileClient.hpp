#ifndef _MOBILE_CLIENT_HPP_
#define _MOBILE_CLIENT_HPP_
#include "NetConfAgent.hpp"
class MobileClient {
public:
    MobileClient();
    bool registerClient(const std::string& number);
    void setName(const std::string& name);

private:
    std::unique_ptr<NetConfAgent> agent;
    std::string userName;
    std::string clientNumber;
};
#endif