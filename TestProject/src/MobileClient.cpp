#include "MobileClient.hpp"
#include <string>
#include <regex>

MobileClient::MobileClient()
{
    _agent = std::make_shared<NetConfAgent>();
    _agent->initSysrepo();
    _register = false;
    _call = false;
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
    if(isRegister())
    {
        std::cout << "Invalid operation" << std::endl;
        return false;
    }
    if(!_userName.empty())
    {
        _register = true;
        _number = number;
        std::string xpath = startxPath + _number + endxPath;
        std::map<std::string, std::string> userName = {{startxPath + _number + endxPathUserName, _userName}};
        _agent->changeData(startxPath + _number + endxPathState, "idle");
        //_agent->registerOperData(moduleName, xpath, userName, *this);
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

bool MobileClient::isCall()
{
    return _call;
}

void MobileClient::handleModuleChange(sysrepo::S_Change change)
{

    if (nullptr != change->new_val()) 
    {
        std::string xpath = change->new_val()->to_string();
        if(xpath.find("state") != -1 && nullptr != change->old_val())
        {
            std::string newState(change->new_val()->data()->get_enum());
            std::string oldState(change->old_val()->data()->get_enum());

            if(newState == "idle" && oldState == "active")
            {
                _call = false;
                std::cout << "Call rejected" << std::endl;
            }
            else if(newState == "idle" && oldState == "busy")
            {
                _call = false;
                std::cout << "Ended call" << std::endl;
            }
            else if(newState == "busy" && oldState == "active")
            {
                std::cout << "Start call" << std::endl;
            }
            else if(newState == "active" && oldState == "idle" && !_abbonentB.empty())
            {
                _call = true;
                std::cout << "You are calling" << std::endl;
            }
        }
        else if (xpath.find("incomingNumber") != -1) 
        {
            _call = true;
            std::cout << std::string(change->new_val()->data()->get_string()) << " is calling [answer/reject]" << std::endl;
        }
    }
}

void MobileClient::handleOperData(libyang::S_Context& ctx, libyang::S_Data_Node &parent, const std::map<std::string, std::string>& data)
{
    if(isRegister())
    {
        for(auto &d: data)
        {
            parent->new_path(ctx, d.first.c_str(), d.second.c_str(), LYD_ANYDATA_CONSTSTRING, 0);
        }
    }
}

void MobileClient::setName(const std::string& name)
{
    if(!isRegister())
    {
        _userName = name;
    }
    else
    {
        std::cout << "You aren't change name, because you registered" << std::endl;
    }
}

void MobileClient::call(const std::string& number)
{
    if(!isRegister())
    {
        std::cout << "You are not registered" << std::endl;
        return;
    }
    if(isCall())
    {
        std::cout << "You are calling another person" << std::endl;
        return;
    }

    if(number != _number)
    {
        std::map<std::string, std::string> data;
        if(!_agent->fetchData(startxPath + number + endxPathState, data))
        {
            std::cout << "Abbonent doesn't exist" << std::endl;
            return;
        }
        _agent->fetchData(startxPath + _number + endxPathState, data);
        if(data[startxPath + number + endxPathState] == "idle")
        {
            _abbonentB = number;
            _call = true;
            _agent->changeData(startxPath + number + endxPathIncNumb, _number);
            _agent->changeData(startxPath + number + endxPathState, "active");
            _agent->changeData(startxPath + _number + endxPathState, "active");
            std::cout << "Calling " << _abbonentB << ". Wait for answer" << std::endl;
        }
        else
        {
            std::cout << "Abbonent is busy" << std::endl;
        }
    }
    else
    {
        std::cout << "You are calling yourself" << std::endl;
    }
}

void MobileClient::answer()
{
    if(!isRegister())
    {
        std::cout << "You are not registered" << std::endl;
        return;
    }
    if(!isCall())
    {
        std::cout << "Not call" << std::endl;
        return;
    }

    if(_abbonentB.empty())
    {
        std::map<std::string, std::string> data;
        _agent->fetchData(startxPath + _number + endxPathState, data);
        
        if(data[startxPath + _number + endxPathState] == "active")
        {
            _agent->fetchData(startxPath + _number + endxPathIncNumb, data);
            _agent->changeData(startxPath + _number + endxPathState, "busy");
            _agent->changeData(startxPath + data[startxPath + _number + endxPathIncNumb] + endxPathState, "busy");         
        }
    }
    else
    {
        std::cout << "Wait for answer" << std::endl;
    }
    
}

void MobileClient::reject()
{
    if(!isRegister())
    {
        std::cout << "You are not registered" << std::endl;
        return;
    }
    if(!isCall())
    {
        std::cout << "Not call" << std::endl;
        return;
    }
    if(_abbonentB.empty())
    {
        std::map<std::string, std::string> data;
        _agent->fetchData(startxPath + _number + endxPathState, data);
        if(data[startxPath + _number + endxPathState] == "active")
        {
            _agent->fetchData(startxPath + _number + endxPathIncNumb, data);
            _agent->changeData(startxPath + _number + endxPathState, "idle");
            _agent->changeData(startxPath + data[startxPath + _number + endxPathIncNumb] + endxPathState, "idle");
            _agent->deleteData(startxPath + _number + endxPathIncNumb);
        }
    }
    else
    {
        std::map<std::string, std::string> data;
        _agent->fetchData(startxPath + _number + endxPathState, data);
        if(data[startxPath + _number + endxPathState] == "active")
        {
            _agent->changeData(startxPath + _number + endxPathState, "idle");
            _agent->changeData(startxPath + _abbonentB + endxPathState, "idle");
            _agent->deleteData(startxPath + _abbonentB + endxPathIncNumb);
            _abbonentB.clear();
        }
    }
}

void MobileClient::endCall()
{
    if(!isRegister())
    {
        std::cout << "You are not registered" << std::endl;
        return;
    }
    if(!isCall())
    {
        std::cout << "Not call" << std::endl;
        return;
    }
    std::map<std::string, std::string> data;
    _agent->fetchData(startxPath + _number + endxPathState, data);
    if(data[startxPath + _number + endxPathState] == "busy")
    {
        if(_abbonentB.empty())
        {
            _agent->fetchData(startxPath + _number + endxPathIncNumb, data);
            _agent->changeData(startxPath + _number + endxPathState, "idle");
            _agent->changeData(startxPath + data[startxPath + _number + endxPathIncNumb] + endxPathState, "idle");
            _agent->deleteData(startxPath + _number + endxPathIncNumb);
        }
        else
        {
            _agent->changeData(startxPath + _number + endxPathState, "idle");
            _agent->changeData(startxPath + _abbonentB + endxPathState, "idle");
            _agent->deleteData(startxPath + _abbonentB + endxPathIncNumb);
            _abbonentB.clear();
        }
    }
}

void MobileClient::unregister()
{
    if(!isRegister())
    {
        std::cout << "You are not registered" << std::endl;
        return;
    }
    if(isCall())
    {
        std::cout << "You are calling" << std::endl;
        return;
    }
    _agent->deleteData(startxPath + _number + endxPath);
    _register = false;
    _userName.clear();
    _number.clear();
}
// Debug function
void MobileClient::unregister(const std::string& number)
{
    _agent->deleteData(startxPath + number + endxPath);
}


