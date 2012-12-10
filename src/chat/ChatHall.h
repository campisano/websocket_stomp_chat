#ifndef ChatHall_H_
#define ChatHall_H_

#include <map>
#include <string>
#include "IChatHandler.h"

struct ChatUserData;
typedef std::pair<unsigned int, ChatUserData> ChatUser;
typedef std::map<unsigned int, ChatUserData> ChatUsers;
typedef std::map<unsigned int, ChatUserData>::iterator ChatUserIterator;

class ChatHall
{
public:
    explicit ChatHall();
    virtual ~ChatHall();

    void setHandler(IChatHandler& _chat_handler);
    bool join(unsigned int _id, std::string _username, std::string _channel);
    bool leave(unsigned int _id);
    bool message(unsigned int _id, std::string _channel, std::string _message);

private:
    IChatHandler* m_chat_handler;
    ChatUsers m_users;
};

#endif
