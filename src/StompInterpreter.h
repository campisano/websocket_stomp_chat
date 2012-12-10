#ifndef StompInterpreter_H_
#define StompInterpreter_H_

#include <string>
#include "websocket/ISocketHandler.h"
#include "websocket/Websocket.h"
#include "stomp/StompProtocol.h"
#include "login/LoginHandler.h"
#include "chat/IChatHandler.h"
#include "chat/ChatHall.h"

struct StompSubscriberData;
typedef std::pair<unsigned int, StompSubscriberData> StompSubscriber;
typedef std::map<unsigned int, StompSubscriberData> StompSubscribers;
typedef std::map<unsigned int, StompSubscriberData>::iterator StompSubscriberIterator;

class StompInterpreter : public ISocketHandler, public IChatHandler
{
public:
    explicit StompInterpreter(Websocket& _websocket, LoginHandler& _login, ChatHall& _chat);
    virtual ~StompInterpreter();

    void connect(unsigned int _id);
    void disconnect(unsigned int _id);
    void receive(unsigned int _id, std::string _data);
    void receive(unsigned int _id, const char* _data, int _size);
    void send(unsigned int _id, std::string _channel, std::string _message);

private:
    Websocket& m_websocket;
    LoginHandler& m_login;
    ChatHall& m_chat;
    StompSubscribers m_subscribers;
    unsigned int m_messageid;
};

#endif
