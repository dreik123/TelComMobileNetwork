#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include "MobileClient.hpp"
#include <memory>

/*bool isFind(std::string str, std::string command)
{
    return str == command;
}*/
bool isRightNumber(std::string number)
{
    auto regex = std::regex("^\\+380[0-9]{9}$");
    return std::regex_match(number, regex);
}
auto registerFunc = [](const std::vector<std::string>& commandArgs, MobileClient& client)
{
    if(commandArgs.size() > 1)
    {
        if(isRightNumber(commandArgs.at(1)))
        {
            client.registerClient(commandArgs.at(1));
        }
        else
        {
            std::cout << "Invalid number. Example: +380435676789" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid command. Must be \"register [arg1]\" " << std::endl;
    }
};

auto nameFunc = [](const std::vector<std::string>& commandArgs, MobileClient& client)
{
    if(commandArgs.size() > 1)
    {
        client.setName(commandArgs.at(1));
        std::cout << "named" << std::endl;
    }
    else
    {
        std::cout << "Invalid command. Must be \"name [arg1]\" " << std::endl;
    }
};

auto callFunc = [](const std::vector<std::string>& commandArgs, MobileClient& client)
{
    if(commandArgs.size() > 1)
    {
        if(isRightNumber(commandArgs.at(1)))
        {
            client.call(commandArgs.at(1));
        }
        else
        {
            std::cout << "Invalid number. Example: +380435676789" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid command. Must be \"call [arg1]\" " << std::endl;
    }
};

auto unregisterFunc = [](const std::vector<std::string>& commandArgs, MobileClient& client)
{
    client.unregister();
};

auto rejectFunc = [](const std::vector<std::string>& commandArgs, MobileClient& client)
{
    client.reject();
};

auto answerFunc = [](const std::vector<std::string>& commandArgs, MobileClient& client)
{
    client.answer();
};

auto callEndFunc = [](const std::vector<std::string>& commandArgs, MobileClient& client)
{
    client.endCall();
};

std::map<std::string, std::function<void(const std::vector<std::string>&, MobileClient&)>> commandsFunc = {
    {"register", registerFunc},
    {"name", nameFunc},
    {"call", callFunc},
    {"unregister", unregisterFunc},
    {"reject", rejectFunc},
    {"answer", answerFunc},
    {"callEnd", callEndFunc}
};
int main()
{
    MobileClient client;
    while(true)
    {
        std::vector<std::string> commandArgs;
        std::string command;

        std::getline(std::cin, command);
        if(command.size() == 0)
        {
            continue;
        }
        std::istringstream iss(command);
        std::string arg;
        while(std::getline(iss, arg, ' '))
        {
            commandArgs.push_back(arg);
        }
        if(commandArgs.at(0) == "exit")
        {
            std::cout << "exit programm" << std::endl;
            break;
        }
        else
        {
            if(commandsFunc.find(commandArgs.at(0)) != commandsFunc.end())
            {
                commandsFunc.at(commandArgs.at(0))(commandArgs, client);
            }
            else
            {
                std::cout << "invalid operation" << std::endl;
            }
        }
    }
    return 0;
}