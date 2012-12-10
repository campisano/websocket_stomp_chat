#include "LoginHandler.h"

LoginHandler::LoginHandler()
{

}

LoginHandler::~LoginHandler()
{

}

bool LoginHandler::login(std::string _username, std::string _password, std::string& _result_session)
{
    _result_session = "abc123def";

    return true;
}

bool LoginHandler::logout(std::string _session)
{
    return true;
}
