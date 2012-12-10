#ifndef Websocket_H_
#define Websocket_H_

#include <map>
#include <string>

#include "ISocketHandler.h"


struct WebsocketUserData;
typedef std::pair<unsigned int, WebsocketUserData> WebsocketUser;
typedef std::map<unsigned int, WebsocketUserData> WebsocketUsers;
typedef std::map<unsigned int, WebsocketUserData>::iterator WebsocketUsersIterator;

class Websocket
{
public:
    explicit Websocket(unsigned int _port);
    virtual ~Websocket();

    bool run(ISocketHandler& _socket_handler);
    void send(unsigned int _id, std::string _data);
    void disconnect(unsigned int _id);

private:
    bool m_keep_running;
    unsigned int m_port;
    WebsocketUsers m_users;
    unsigned int m_user_counter;
};

#endif
