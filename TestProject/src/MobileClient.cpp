#include "MobileClient.hpp"

MobileClient::MobileClient()
{
    agent = std::make_unique<NetConfAgent>();
    agent->initSysrepo();
}

bool MobileClient::registerClient(const std::string& number)
{
    clientNumber = number;
    agent->subscribeForModelChanges("mobile-network");
    agent->changeData("/mobile-network:core/subscribers[number='" + number + "']/state", "idle");
    return true;
}

