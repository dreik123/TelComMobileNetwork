#include "MobileClient.hpp"
#include <string>
MobileClient::MobileClient()
{
    _agent = std::make_shared<NetConfAgent>();
    _agent->initSysrepo();
}
namespace
{
    const std::string moduleName = "mobile-network";
    const std::string startxPath = "/mobile-network:core/subscribers[number='";
    const std::string endxPath = "']";
    const std::string endxPathUserName = "']/userName";
    const std::string endxPathIncNumb = "']/incomingNumber";
    const std::string endxPathState = "']/state";
}

bool MobileClient::registerClient(const std::string& number)
{
    if(!_userName.empty())
    {
        _number = number;
        std::string xpath = startxPath + _number + endxPath;
        std::map<std::string, std::string> userName = {{startxPath + _number + endxPathUserName, _userName}};
        _agent->changeData(startxPath + _number + endxPathState, "idle");
        _agent->registerOperData(moduleName, xpath, userName, *this);
        _agent->subscribeForModelChanges(moduleName, xpath, *this);
    }
    else
    {
        std::cout << "Need set Name" << std::endl;
    }
    return true;
}

void MobileClient::handleModuleChange()
{
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
    std::map<std::string, std::string> result;
    _agent->fetchData(startxPath + number + endxPathState, result);
    _agent->fetchData(startxPath + _number + endxPathState, result);
    if(result[startxPath + number + endxPathState] == "idle"
    && result[startxPath + _number + endxPathState] == "idle")
    {
        _abbonentB = number;
        _agent->changeData(startxPath + number + endxPath, _number);
        _agent->changeData(startxPath + number + endxPathState, "active");
        _agent->changeData(startxPath + _number + endxPathState, "active");
    }
    else
    {
        std::cout << "Abbonent is busy or you are calling another person" << std::endl;
    }

}

void MobileClient::answer


