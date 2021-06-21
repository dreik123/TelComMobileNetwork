#include "MobileClient.hpp"
#include <string>
MobileClient::MobileClient()
{
    _agent = std::make_shared<NetConfAgent>();
    _agent->initSysrepo();
    _register = false;
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
        _register = true;
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
bool MobileClient::isRegister()
{
    return _register;
}
void MobileClient::handleModuleChange()
{

}
void MobileClient::handleOperData()
{
}

void MobileClient::setName(const std::string& name)
{
    _userName = name;
}

void MobileClient::call(const std::string& number)
{
    std::map<std::string, std::string> data;
    _agent->fetchData(startxPath + number + endxPathState, data);
    _agent->fetchData(startxPath + _number + endxPathState, data);
    if(data[startxPath + number + endxPathState] == "idle"
    && data[startxPath + _number + endxPathState] == "idle")
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

void MobileClient::answer()
{
    if(_abbonentB.empty())
    {
        std::map<std::string, std::string> data;
        _agent->fetchData(startxPath + _number + endxPathState, data);
        _agent->fetchData(startxPath + _number + endxPathIncNumb, data);
        if(data[startxPath + _number + endxPathState] == "active")
        {
            _agent->changeData(startxPath + _number + endxPathState, "busy");
            _agent->changeData(startxPath + data[startxPath + _number + endxPathIncNumb] + endxPathState, "busy");
        }
    }
    
}

void MobileClient::reject()
{
    if(_abbonentB.empty())
    {
        std::map<std::string, std::string> data;
        _agent->fetchData(startxPath + _number + endxPathState, data);
        _agent->fetchData(startxPath + _number + endxPathIncNumb, data);
        if(data[startxPath + _number + endxPathState] == "active")
        {
            _agent->changeData(startxPath + _number + endxPathState, "idle");
            _agent->changeData(startxPath + data[startxPath + _number + endxPathIncNumb] + endxPathState, "idle");
            _agent->changeData(startxPath + _number + endxPathIncNumb);
        }
    }
}

void MobileClient::endCall()
{
    if(_abbonentB.empty())
    {
        std::map<std::string, std::string> data;
        _agent->fetchData(startxPath + _number + endxPathState, data);
        _agent->fetchData(startxPath + _number + endxPathIncNumb, data);
        if(data[startxPath + _number + endxPathState] == "busy")
        {
            _agent->changeData(startxPath + _number + endxPathState, "idle");
            _agent->changeData(startxPath + data[startxPath + _number + endxPathIncNumb] + endxPathState, "idle");
            _agent->changeData(startxPath + _number + endxPathIncNumb);
        }
    }
    else
    {
        _agent->changeData(startxPath + _number + endxPathState, "idle");
        _agent->changeData(startxPath + _abbonentB + endxPathState, "idle");
        _agent->changeData(startxPath + _abbonentB + endxPathIncNumb);
        _abbonentB = "";
    }
}

void MobileClient::unregister()
{
    std::map<std::string, std::string> data;
    _agent->fetchData(startxPath + _number + endxPathState, data);
    if(data[startxPath + _number + endxPathState] == "idle")
    {
        _agent->changeData(startxPath + _number + endxPathState);
        _agent->changeData(startxPath + _number + endxPath);
        _register = false;
    }
}


