#include <memory>
#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/Connection.hpp>
#include <string>
#include <vector>
#include <map>
class NetConfAgent {
public:
    bool initSysrepo();
    bool fetchData(const std::string& xpath, std::map<std::string, std::string>& res);
    bool subscribeForModelChanges(const std::string& module);
    bool registerOperData(const std::string& module, const std::string& xpath);
    bool subscribeForRpc(const std::string& xpath);
    bool notifySysrepo(const std::string& xpath, std::map<std::string, std::string>& values);
    bool changeData(const std::string& xpath, std::string values);
private:
    std::unique_ptr<sysrepo::Session> sess;
    std::unique_ptr<sysrepo::Connection> conn;
};