#include <gmock/gmock.h>
#include "NetConfAgent.hpp"
#include <string>
#include <map>

class NetConfAgentMock : public NetConfAgent
{
public:
    
    MOCK_METHOD(bool, initSysrepo,(), (override));
    MOCK_METHOD(bool, fetchData,((const std::string& xpath), (std::map<std::string, std::string>& res)), (override));
    MOCK_METHOD(bool, subscribeForModelChanges,((const std::string& module), (const std::string& xpath), (MobileClient& client)), (override));
    MOCK_METHOD(bool, registerOperData, 
    ((const std::string& module), (const std::string& xpath), (const std::map<std::string, std::string>& data), (MobileClient& client)), (override));
    MOCK_METHOD(bool, subscribeForRpc, ((const std::string& xpath), (const std::map<std::string, std::string>& output_val_for_rpc)), (override));
    MOCK_METHOD(bool, notifySysrepo, ((const std::string& xpath), (const std::map<std::string, std::string>& values)), (override));
    MOCK_METHOD(bool, changeData, ((const std::string& xpath), (const std::string& value)), (override));
    MOCK_METHOD(bool, deleteData, ((const std::string& xpath)), (override));
};