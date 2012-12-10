#ifndef LoginHandler_H_
#define LoginHandler_H_

#include <string>

class LoginHandler
{
public:
    explicit LoginHandler();
    virtual ~LoginHandler();

    bool login(std::string _username, std::string _password, std::string& _result_session);
    bool logout(std::string _session);

private:
};

#endif
