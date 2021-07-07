#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "NetConfAgentMock.hpp"
#include "../include/MobileClient.hpp"
#include <string>

using testing::_;
using testing::Return;
class MobileClientTest : public testing::Test
{
    protected:
    void SetUp() override
    {
        auto temp = std::make_unique<testing::StrictMock<NetConfAgentMock>>();
        EXPECT_CALL(*temp, initSysrepo()).WillOnce(Return(true));
        _mock = temp.get();
        _mobileClient = std::make_unique<MobileClient>(std::move(temp));
        
    }
    protected:
    std::unique_ptr<MobileClient> _mobileClient;
    testing::StrictMock<NetConfAgentMock> *_mock;
};
namespace
{
    const std::string moduleName = "mobile-network";
    const std::string startxPath = "/mobile-network:core/subscribers[number='";
    const std::string endxPath = "']";
    const std::string endxPathUserName = "']/userName";
    const std::string endxPathIncNumb = "']/incomingNumber";
    const std::string endxPathState = "']/state";

    const std::string number1 = "+380111111111";
    const std::string number2 = "+380222222222";
    const std::string stateIdle = "idle";
    const std::string userName = "fefe";
    
}
TEST_F(MobileClientTest, shouldSuccedToRegister)
{
    
    _mobileClient->setName(userName);   
    //_mobileClient->isRegister();
    std::map<std::string, std::string> data {{startxPath + number1 + endxPathUserName, userName}};
    std::map<std::string, std::string> data2;
    EXPECT_EQ(_mobileClient->isRegister(), false);
    EXPECT_CALL(*_mock, fetchData(startxPath + number1 + endxPathState, data2)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, changeData(startxPath + number1 + endxPathState, stateIdle)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, registerOperData(moduleName, startxPath + number1 + endxPath, data, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, subscribeForModelChanges(moduleName, startxPath + number1 + endxPath, _)).WillOnce(Return(true));
    _mobileClient->registerClient(number1);
    

}

/*TEST_F(MobileClientTest, shouldSuccedToCall)
{
        
    _mobileClient->setName(userName); 
    std::map<std::string, std::string> data {{startxPath + number1 + endxPathUserName, userName}};
    std::map<std::string, std::string> data2;
    EXPECT_CALL(*_mock, changeData(startxPath + number1 + endxPathState, stateIdle)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, registerOperData(moduleName, startxPath + number1 + endxPath, data, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, subscribeForModelChanges(moduleName, startxPath + number1 + endxPath, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, fetchData(startxPath + number1 + endxPathState, data2)).WillOnce(Return(true));
    _mobileClient->registerClient(number1);
    EXPECT_CALL(*_mock, fetchData(startxPath + number1 + endxPathState, data2)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, fetchData(startxPath + number2 + endxPathState, data2)).WillOnce(Return(true));

    //EXPECT_CALL(*_mock, changeData(startxPath + number2 + endxPathIncNumb, number1)).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, changeData(startxPath + number2 + endxPathState, "active")).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, changeData(startxPath + number1 + endxPathState, "active")).WillOnce(Return(true));
    EXPECT_EQ(_mobileClient->isRegister(), true);
    _mobileClient->call(number2);
}

TEST_F(MobileClientTest, shouldSuccedToAnswer)
{
    _mobileClient->setName(userName);   
    _mobileClient->isRegister();
    std::map<std::string, std::string> data {{startxPath + number1 + endxPathUserName, userName}};
    std::map<std::string, std::string> data2;
    //EXPECT_CALL(*_mock, changeData(startxPath + number1 + endxPathState, stateIdle)).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, registerOperData(moduleName, startxPath + number1 + endxPath, data, _)).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, subscribeForModelChanges(moduleName, startxPath + number1 + endxPath, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, fetchData(startxPath + number1 + endxPathState, data2)).WillOnce(Return(true));
    _mobileClient->registerClient(number1);

    _mobileClient->isCall();
    //EXPECT_CALL(*_mock, fetchData(startxPath + number1 + endxPathState, data2)).WillOnce(Return(true));
    _mobileClient->answer();
}

TEST_F(MobileClientTest, shouldSuccedToReject)
{
     _mobileClient->setName(userName);   
    _mobileClient->isRegister();
    std::map<std::string, std::string> data {{startxPath + number1 + endxPathUserName, userName}};
    std::map<std::string, std::string> data2;
    //EXPECT_CALL(*_mock, changeData(startxPath + number1 + endxPathState, stateIdle)).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, registerOperData(moduleName, startxPath + number1 + endxPath, data, _)).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, subscribeForModelChanges(moduleName, startxPath + number1 + endxPath, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, fetchData(startxPath + number1 + endxPathState, data2)).WillOnce(Return(true));;
    _mobileClient->registerClient(number1);
    _mobileClient->isCall();
    _mobileClient->reject();
}

TEST_F(MobileClientTest, shouldSuccedToEndCall)
{
         _mobileClient->setName(userName);   
    _mobileClient->isRegister();
    std::map<std::string, std::string> data {{startxPath + number1 + endxPathUserName, userName}};
    std::map<std::string, std::string> data2;
    //EXPECT_CALL(*_mock, changeData(startxPath + number1 + endxPathState, stateIdle)).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, registerOperData(moduleName, startxPath + number1 + endxPath, data, _)).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, subscribeForModelChanges(moduleName, startxPath + number1 + endxPath, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, fetchData(startxPath + number1 + endxPathState, data2)).WillOnce(Return(true));
    _mobileClient->registerClient(number1);
    _mobileClient->isCall();
    _mobileClient->endCall();
}

TEST_F(MobileClientTest, shouldSuccedToUnregister)
{
             _mobileClient->setName(userName);   
    _mobileClient->isRegister();
    std::map<std::string, std::string> data {{startxPath + number1 + endxPathUserName, userName}};
    std::map<std::string, std::string> data2;
    //EXPECT_CALL(*_mock, changeData(startxPath + number1 + endxPathState, stateIdle)).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, registerOperData(moduleName, startxPath + number1 + endxPath, data, _)).WillOnce(Return(true));
    //EXPECT_CALL(*_mock, subscribeForModelChanges(moduleName, startxPath + number1 + endxPath, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, fetchData(startxPath + number1 + endxPathState, data2)).WillOnce(Return(true));
    _mobileClient->registerClient(number1);
    _mobileClient->isCall();
    //EXPECT_CALL(*_mock, deleteData(startxPath + number1 + endxPath)).WillOnce(Return(true));
    _mobileClient->unregister();
}*/