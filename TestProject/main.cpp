#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
bool isFind(std::string str, std::string command)
{
    return str == command;
}
bool isRightNumber(std::string number)
{
    auto regex = std::regex("^\\+380[0-9]{9}$");
    return std::regex_match(number, regex);
}
int main()
{
    
    while(true)
    {
        std::vector<std::string> commandArgs;
        std::string command;
        
        std::getline(std::cin, command);
        std::istringstream iss(command);
        std::string arg;
        while(std::getline(iss, arg, ' '))
        {
            commandArgs.push_back(arg);
        }
        if(isFind("unregister", commandArgs.at(0)))
        {
            std::cout << "unregistered" << std::endl;
        }
        else if(isFind("register", commandArgs.at(0)))
        {
            if(commandArgs.size() > 1)
            {
                if(isRightNumber(commandArgs.at(1)))
                {
                    std::cout << "registered" << std::endl;
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
        }
        else if(isFind("name", commandArgs.at(0)))
        {
            if(commandArgs.size() > 1)
            {
                std::cout << "named" << std::endl;
            }
            else
            {
                std::cout << "Invalid command. Must be \"name [arg1]\" " << std::endl;
            }
        }
        else if(isFind("callEnd", commandArgs.at(0)))
        {
            std::cout << "end call" << std::endl;
        }
        else if(isFind("call", commandArgs.at(0)))
        {
            if(commandArgs.size() > 1)
            {
                if(isRightNumber(commandArgs.at(1)))
                {
                    std::cout << "calling" << std::endl;
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
        }
        else if(isFind("answer", commandArgs.at(0)))
        {
            std::cout << "answered call" << std::endl;
        }
        else if(isFind("reject", commandArgs.at(0)))
        {
            std::cout << "rejected call" << std::endl;
        }
        else if(isFind("exit", commandArgs.at(0)))
        {
            std::cout << "exit programm" << std::endl;
            break;
        }
        else 
        {
            std::cout << "invalid operation" << std::endl;
        }
    }
    return 0;
}