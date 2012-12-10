#ifndef StompProtocol_H_
#define StompProtocol_H_

#include <map>
#include <sstream>
#include <string>

//from http://stomp.github.com//stomp-specification-1.1.html
#define STOMP_VERSION 1.1

namespace Stomp
{
enum Command
{
    UNKNOWN,
    // client commands
    CONNECT,
    SEND,
    SUBSCRIBE,
    UNSUBSCRIBE,
    BEGIN,
    COMMIT,
    ABORT,
    ACK,
    DISCONNECT,
    // server commands
    CONNECTED,
    MESSAGE,
    RECEIPT,
    ERROR

};

typedef std::pair<std::string, std::string> Header;
typedef std::map<std::string, std::string> Headers;
typedef std::map<std::string, std::string>::iterator HeaderIterator;
typedef std::stringstream Body;

class Frame
{
public:
    explicit Frame(std::string _frame_data);
    explicit Frame(const char* _frame_data, int _size);
    explicit Frame();
    virtual ~Frame();

    const Command& getCommand() const;
    void setCommand(const Command& _command);
    const Body& getBody() const;
    void setBody(const Body& _body);
    void setBody(const char* _body_data, int _size);
    void addOrReplaceHeader(Header _header);
    std::string getHeaderValueFor(std::string _header_name);
    std::string getFrameData();

private:
    Command m_command;
    Headers m_headers;
    Body m_body;
};
}
#endif
