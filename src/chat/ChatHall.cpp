#include "ChatHall.h"

struct ChatUserData
{
    std::string username;
    std::string channel;
};

ChatHall::ChatHall() : m_chat_handler(NULL)
{

}

ChatHall::~ChatHall()
{

}

void ChatHall::setHandler(IChatHandler& _chat_handler)
{
    m_chat_handler = &_chat_handler;
}

bool ChatHall::join(unsigned int _id, std::string _username, std::string _channel)
{
    ChatUserIterator it = m_users.find(_id);

    if(it != m_users.end())
    {
        return false;
    }

    ChatUserData data = { _username, _channel };
    m_users.insert(ChatUser(_id, data));

    return true;
}

bool ChatHall::leave(unsigned int _id)
{
    ChatUserIterator it = m_users.find(_id);

    if(it == m_users.end())
    {
        return false;
    }

    m_users.erase(it);

    return true;
}

bool ChatHall::message(unsigned int _id, std::string _channel, std::string _message)
{
    ChatUserIterator it_sender = m_users.find(_id);

    if(it_sender == m_users.end())
    {
        return false;
    }

    ChatUserIterator it;

    for(it = m_users.begin(); it != m_users.end(); ++it)
    {
        if(_channel == it->second.channel)
        {
            m_chat_handler->send(it->first, _channel, it->second.username + ":" + _message);
        }
    }

    return true;
}
