#include "MobileClient.hpp"
#include <string>
#include <regex>
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
void MobileClient::handleModuleChange(sysrepo::S_Change change)
{
    switch(change->oper())
    {
        case SR_OP_CREATED:
        {
            std::cout << "created" << std::endl;
            if (nullptr != change->new_val()) 
            {
                /*std::string xpath = change->new_val()->to_string();
                auto regex = std::regex("incomingNumber");
                if(std::regex_match(std::string(xpath), regex))
                {
                    std::map<std::string, std::string> userName;
                    std::string xPath = startxPath + std::string(change->new_val()->data()->get_string()) + endxPathUserName;
                    _agent->fetchData(xPath, userName);
                    std::cout << userName[xPath] << "is calling" << std::endl;

                }*/
                /*std::map<std::string, std::string> userName;
                        std::string xPathIncNumb = startxPath + _number + endxPathIncNumb;
                        _agent->fetchData(xPathIncNumb, userName);
                        std::string xPathUserName = startxPath + userName[xPathIncNumb] + endxPathIncNumb;
                        _agent->fetchData(xPathUserName, userName);
                        std::cout << userName[xPathUserName] << "is calling" << std::endl;*/
                        std::cout << std::string(change->new_val()->data()->get_string()) << "is calling" << std::endl;
            }
            break;
        }
        case SR_OP_MODIFIED:
        {
            std::cout << "modified" << std::endl;
            if (nullptr != change->new_val() && nullptr != change->old_val()) 
            {
                //std::cout << "old val:" << change->new_val()->to_string() << std::endl;
                //std::cout << "new val:" << change->old_val()->to_string() << std::endl;
                std::string xpath = change->new_val()->to_string();
                //auto regex = std::regex("state");
                //if(std::regex_match(std::string(xpath), regex))
                //{
                    std::string newState(change->new_val()->data()->get_enum());
                    std::string oldState(change->old_val()->data()->get_enum());
                    if(newState == "idle" && oldState == "active" && !_abbonentB.empty())
                    {
                        /*std::map<std::string, std::string> userName;
                        std::string xPath = startxPath + _abbonentB + endxPathUserName;
                        _agent->fetchData(xPath, userName);
                        std::cout << userName[xPath] << "rejected call" << std::endl;
                        _abbonentB = "";*/
                        std::cout << _abbonentB << "rejected call" << std::endl;
                        _abbonentB = "";
                    }
                    else if(newState == "idle" && oldState == "active")
                    {
                        std::cout << "You rejected call" << std::endl;
                    }
                    else if(newState == "idle" && oldState == "busy")
                    {
                        std::cout << "Ended call" << std::endl;
                    }
                    else if(newState == "busy" && oldState == "active")
                    {
                        std::cout << "Start call" << std::endl;
                    }
                //}
            }
            break;
        }
        case SR_OP_DELETED:
        {
            std::cout << "deleted" << std::endl;
            break;
        }
        default:
        {
            break;
        }
    }
}
void MobileClient::handleOperData(libyang::S_Context& ctx, libyang::S_Data_Node &parent, const std::map<std::string, std::string>& data)
{
    for(auto &d: data)
    {
        parent->new_path(ctx, d.first.c_str(), d.second.c_str(), LYD_ANYDATA_CONSTSTRING, 0);
    }
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
        _agent->changeData(startxPath + number + endxPathIncNumb, _number);
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
        _userName = "";
        _number = "";
    }
}


