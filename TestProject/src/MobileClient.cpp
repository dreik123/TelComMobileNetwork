#include "MobileClient.hpp"

MobileClient::MobileClient()
{
    _agent = std::make_shared<NetConfAgent>();
    _agent->initSysrepo();
}

bool MobileClient::registerClient(const std::string& number)
{
    _number = number;
    std::string xpath = "/mobile-network:core/subscribers[number='" + _number + "']";
    _agent->registerOperData("mobile-network", xpath, this);
    _agent->changeData(xpath + "/state", "idle");
    _agent->subscribeForModelChanges("mobile-network", this);
    return true;
}

void MobileClient::handleModuleChange()
{
    std::cout << "module change" << std::endl;
}

void MobileClient::handleOperData()
{
    std::cout << "oper data" << std::endl;
}

