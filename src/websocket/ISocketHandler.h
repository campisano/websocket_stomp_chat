#ifndef ISocketHandler_H_
#define ISocketHandler_H_

#include <string>

class ISocketHandler
{
public:
    explicit ISocketHandler() {};
    virtual ~ISocketHandler() {};

    virtual void connect(unsigned int _id) = 0;
    virtual void disconnect(unsigned int _id) = 0;
    virtual void receive(unsigned int _id, std::string _data) = 0;
    virtual void receive(unsigned int _id, const char* _data, int _size) = 0;
};

#endif
