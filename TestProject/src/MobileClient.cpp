#include "MobileClient.hpp"

MobileClient::MobileClient()
{
    _agent = std::make_shared<NetConfAgent>();
    _agent->initSysrepo();
}

bool MobileClient::registerClient(const std::string& number)
{
    if(!_userName.empty())
    {
        _number = number;
        std::string xpath = "/mobile-network:core/subscribers[number='" + _number + "']";
        std::map<std::string, std::string> userName = {{"/mobile-network:core/subscribers[number='" + _number + "']/userName", _userName}};
        _agent->changeData(xpath + "/state", "idle");
        _agent->registerOperData("mobile-network", xpath, userName, *this);
        _agent->subscribeForModelChanges("mobile-network", xpath, *this);
    }
    else
    {
        std::cout << "Need set Name" << std::endl;
    }
    return true;
}

void MobileClient::handleModuleChange(sysrepo::S_Session sess, sr_event_t event,
    uint32_t request_id)
{
    std::map<std::string, std::string> data;
    _agent->fetchData("/mobile-network:core/subscribers[number='" + _number + "']/incomingNumber", data);
    if(data["/mobile-network:core/subscribers[number='" + _number + "']/incomingNumber"].length() > 0)
    {
        std::cout << "calling" << _number << std::endl;
    }
}

void MobileClient::handleOperData()
{
    std::cout << "oper data" << std::endl;
}

void MobileClient::setName(const std::string& name)
{
    _userName = name;
}

void MobileClient::call(const std::string& number)
{
    std::string xpath = "/mobile-network:core/subscribers[number='" + number + "']";
    std::string xpathSt = xpath + "/state";
    std::string xpathIncNumb = xpath + "/incomingNumber";
    std::map<std::string, std::string> result;
    _agent->fetchData(xpathSt, result);
    if(result[xpathSt] == "idle")
    {
        _agent->changeData(xpathIncNumb, _number);
        _agent->changeData(xpathSt, "active");
        _agent->changeData("/mobile-network:core/subscribers[number='" + _number + "']/state", "active");
    }

}
