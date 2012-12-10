#include <exception>
#include <iostream>

#include "chat/ChatHall.h"
#include "login/LoginHandler.h"
#include "websocket/Websocket.h"
#include "StompInterpreter.h"

int main(void)
{
    try
    {
        LoginHandler login;
        ChatHall chat;
        Websocket websocket(9000);
        StompInterpreter stomp_interpreter(websocket, login, chat);
        chat.setHandler(stomp_interpreter);
        websocket.run(stomp_interpreter);
    }
    catch (std::exception const& _exc)
    {
        std::cerr << _exc.what() << std::endl;

        return 1;
    }

    return 0;
}
