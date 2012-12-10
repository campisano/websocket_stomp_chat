#ifndef IChatHandler_H_
#define IChatHandler_H_

#include <string>

class IChatHandler
{
public:
    explicit IChatHandler() {};
    virtual ~IChatHandler() {};

    virtual void send(unsigned int _id, std::string _channel, std::string _message) = 0;
};

#endif
