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

TEST_F(MobileClientTest, shouldSuccedToRegister)
{
    std::string Name = "fefe";
    _mobileClient->setName(Name);   
    std::string test = "+380111111111";
    std::string state = "idle";
    EXPECT_CALL(*_mock, changeData(test,state)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, registerOperData(_,_,_,_)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, subscribeForModelChanges(_,_,_)).WillOnce(Return(true));
    EXPECT_CALL(*_mock, fetchData(_,_));
    _mobileClient->registerClient(test);
}

TEST_F(MobileClientTest, shouldSuccedToCall)
{
    std::string Name = "fefe";
    _mobileClient->setName(Name);   
    std::string number1 = "+380111111111";
    std::string number2 = "+380222222222";
    
    EXPECT_CALL(*_mock, changeData(_,_));
    EXPECT_CALL(*_mock, registerOperData(_,_,_,_));
    EXPECT_CALL(*_mock, subscribeForModelChanges(_,_,_));
    EXPECT_CALL(*_mock, fetchData(_,_));
    _mobileClient->registerClient(number1);

    
    _mobileClient->call(number2);
}
/*
TEST_F(MobileClientTest, shouldSuccedToAnswer)
{
    std::string test = "1";
    _mobileClient->answer();
}

TEST_F(MobileClientTest, shouldSuccedToReject)
{
    std::string test = "1";
    _mobileClient->reject();
}

TEST_F(MobileClientTest, shouldSuccedToEndCall)
{
    std::string test = "1";
    _mobileClient->endCall();
}

TEST_F(MobileClientTest, shouldSuccedToUnregister)
{
    std::string test = "1";
    _mobileClient->unregister();
}*/