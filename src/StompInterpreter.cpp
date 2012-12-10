#include "StompInterpreter.h"

struct StompSubscriberData
{
    std::string session;
    std::string username;
    std::string subscription;
    std::string destination;
};

#include "stomp/StompProtocol.h"

namespace
{
Websocket* ns_websocket = NULL;
StompSubscribers* ns_subscribers = NULL;

bool ns_subscribeExist(unsigned int _id)
{
    StompSubscriberIterator it = ns_subscribers->find(_id);

    if(it != ns_subscribers->end())
    {
        return true;
    }

    return false;
}

void ns_receipt(unsigned int _id, std::string _receipt_id)
{
    // from http://stomp.github.com//stomp-specification-1.1.html#RECEIPT
    Stomp::Frame send;
    send.setCommand(Stomp::RECEIPT);
    send.addOrReplaceHeader(Stomp::Header("receipt-id", _receipt_id));
    ns_websocket->send(_id, send.getFrameData());
}

void ns_error(unsigned int _id, std::string _receipt_id, std::string _message)
{
    // from http://stomp.github.com//stomp-specification-1.1.html#ERROR
    Stomp::Frame send;
    send.setCommand(Stomp::ERROR);
    send.addOrReplaceHeader(Stomp::Header("receipt-id", _receipt_id));
    send.setBody(_message.c_str(), _message.size());
    ns_websocket->send(_id, send.getFrameData());
}
}

StompInterpreter::StompInterpreter(Websocket& _websocket, LoginHandler& _login, ChatHall& _chat): m_websocket(_websocket), m_login(_login), m_chat(_chat), m_messageid(0)
{
    ns_websocket = &m_websocket;
    ns_subscribers = &m_subscribers;
}

StompInterpreter::~StompInterpreter()
{

}

void StompInterpreter::connect(unsigned int _id)
{
    if(! ns_subscribeExist(_id))
    {
        // from http://stomp.github.com//stomp-specification-1.1.html#Connecting
        StompSubscriberData data = { "", "", "", ""};
        m_subscribers.insert(StompSubscriber(_id, data));
    }
}

void StompInterpreter::disconnect(unsigned int _id)
{
    StompSubscriberIterator it = ns_subscribers->find(_id);

    if(it != ns_subscribers->end())
    {
        m_login.logout(it->second.session);
        m_chat.leave(it->first);
    }
}

void StompInterpreter::receive(unsigned int _id, std::string _data)
{
    receive(_id, _data.c_str(), _data.size());
}

void StompInterpreter::receive(unsigned int _id, const char* _data, int _size)
{
    Stomp::Frame received(_data, _size);

    switch(received.getCommand())
    {
    case Stomp::CONNECT:
    {
        std::string session;
        std::string username = received.getHeaderValueFor("login");
        std::string password = received.getHeaderValueFor("passcode");

        if(m_login.login(username, password, session))
        {
            StompSubscriberIterator it = ns_subscribers->find(_id);

            if(it != ns_subscribers->end())
            {
                // from http://stomp.github.com//stomp-specification-1.1.html#Connecting
                it->second.session = session;
                it->second.username = username;
                std::stringstream ssid;
                ssid << _id;
                Stomp::Frame send;
                send.setCommand(Stomp::CONNECTED);
                send.addOrReplaceHeader(Stomp::Header("session", ssid.str()));
                m_websocket.send(_id, send.getFrameData());
            }
            else
            {
                ns_error(_id, "CONNECT", "unknown internal user id");
            }
        }
        else
        {
            ns_error(_id, "CONNECT", "LoginHandler error");
        }
        break;
    }
    case Stomp::DISCONNECT:
    {
        StompSubscriberIterator it = ns_subscribers->find(_id);
        disconnect(_id);
        m_websocket.disconnect(_id);

        break;
    }
    case Stomp::SUBSCRIBE:
    {
        // from http://stomp.github.com//stomp-specification-1.1.html#SUBSCRIBE
        std::string subscribe_id = received.getHeaderValueFor("id");
        std::string destination = received.getHeaderValueFor("destination");

        StompSubscriberIterator it = ns_subscribers->find(_id);

        if(it != ns_subscribers->end())
        {
            it->second.subscription = subscribe_id;
            it->second.destination = destination;

            if(m_chat.join(_id, it->second.username, destination))
            {
                ns_receipt(_id, subscribe_id);
            }
            else
            {
                ns_error(_id, "SUBSCRIBE", "ChatHall error");
            }
        }
        else
        {
            ns_error(_id, "SUBSCRIBE", "unknown internal user id");
            disconnect(_id);
            m_websocket.disconnect(_id);
        }

        break;
    }
    case Stomp::SEND:
    {
        //from http://stomp.github.com//stomp-specification-1.1.html#SEND
        std::string destination = received.getHeaderValueFor("destination");
        std::string message = received.getBody().str();

        StompSubscriberIterator it = ns_subscribers->find(_id);

        if(it != ns_subscribers->end())
        {
            if(m_chat.message(_id, destination, message))
            {
                ns_receipt(_id, destination);
            }
            else
            {
                ns_error(_id, "SEND", "ChatHall error");
                disconnect(_id);
                m_websocket.disconnect(_id);
            }
        }
        else
        {
            ns_error(_id, "SEND", "unknown internal user id");
            disconnect(_id);
            m_websocket.disconnect(_id);
        }

        break;
    }
    default:
    {
        break;
    }
    }
}

void StompInterpreter::send(unsigned int _id, std::string _channel, std::string _message)
{
    StompSubscriberIterator it = ns_subscribers->find(_id);

    if(it != ns_subscribers->end())
    {
        // from http://stomp.github.com//stomp-specification-1.1.html#MESSAGE
        Stomp::Frame send;
        send.setCommand(Stomp::MESSAGE);
        send.addOrReplaceHeader(Stomp::Header("subscription", it->second.subscription));
        std::stringstream ss_messageid;
        ss_messageid << m_messageid;
        send.addOrReplaceHeader(Stomp::Header("message-id", ss_messageid.str()));
        send.addOrReplaceHeader(Stomp::Header("destination", _channel));
        send.addOrReplaceHeader(Stomp::Header("content-type", "text/plain"));
        send.setBody(_message.c_str(), _message.size());
        m_websocket.send(_id, send.getFrameData());
        ++m_messageid;
    }
}
