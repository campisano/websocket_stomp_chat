#include "StompProtocol.h"

#include <cstring>
#include <exception>
#include <sstream>


namespace
{
class StompException: public std::exception
{
public:
    explicit StompException(const std::string& _what) throw() : m_what(_what) {};
    virtual ~StompException() throw() {};
private:
    virtual const char* what() const throw()
    {
        return m_what.c_str();
    }
    std::string m_what;
};

bool ns_parseCommand(Stomp::Frame& _frame, const char* _frame_data, int _size, const char*& _startline_pointer)
{
    if(_startline_pointer != _startline_pointer || _size <= 0)
    {
        return false;
    }

    int endline_length;
    const char* endline = std::strstr(_frame_data, "\r\n");

    if(endline != NULL)
    {
        endline_length = 2;
    }
    else
    {
        endline = std::strchr(_frame_data, '\n');

        if(endline != NULL)
        {
            endline_length = 1;
        }
        else
        {
            return false;
        }
    }

    std::string command(_startline_pointer, endline - _frame_data);

    // client commands
    if(command == "CONNECT")
    {
        _frame.setCommand(Stomp::CONNECT);
    }
    else if(command == "SEND")
    {
        _frame.setCommand(Stomp::SEND);
    }
    else if(command == "SUBSCRIBE")
    {
        _frame.setCommand(Stomp::SUBSCRIBE);
    }
    else if(command == "UNSUBSCRIBE")
    {
        _frame.setCommand(Stomp::UNSUBSCRIBE);
    }
    else if(command == "BEGIN")
    {
        _frame.setCommand(Stomp::BEGIN);
    }
    else if(command == "COMMIT")
    {
        _frame.setCommand(Stomp::COMMIT);
    }
    else if(command == "ABORT")
    {
        _frame.setCommand(Stomp::ABORT);
    }
    else if(command == "ACK")
    {
        _frame.setCommand(Stomp::ACK);
    }
    else if(command == "DISCONNECT")
    {
        _frame.setCommand(Stomp::DISCONNECT);
    }
    // server commands
    else if(command == "CONNECTED")
    {
        _frame.setCommand(Stomp::CONNECTED);
    }
    else if(command == "MESSAGE")
    {
        _frame.setCommand(Stomp::MESSAGE);
    }
    else if(command == "RECEIPT")
    {
        _frame.setCommand(Stomp::RECEIPT);
    }
    else if(command == "ERROR")
    {
        _frame.setCommand(Stomp::ERROR);
    }
    else
    {
        _frame.setCommand(Stomp::UNKNOWN);
        return false;
    }

    _startline_pointer = endline + endline_length;

    return true;
}

bool ns_parseHeaders(Stomp::Frame& _frame, const char* _frame_data, int _size, const char*& _startline_pointer)
{
    if(_startline_pointer <= _frame_data || (_startline_pointer - _frame_data) >= _size )
    {
        return false;
    }

    int headers_endline_length;
    const char* headers_endline = std::strstr(_startline_pointer, "\r\n\r\n");

    if(headers_endline != NULL)
    {
        headers_endline_length = 4;
    }
    else
    {
        headers_endline = std::strstr(_startline_pointer, "\n\n");

        if(headers_endline != NULL)
        {
            headers_endline_length = 2;
        }
        else
        {
            headers_endline = std::strchr(_startline_pointer, '\0');

            if(headers_endline == NULL)
            {
                return false;
            }

            headers_endline_length = 0;
        }
    }

    const char* startline = _startline_pointer;
    const char* endline;
    int endline_length;
    const char* colon;
    Stomp::Header header;

    while(startline < headers_endline + headers_endline_length)
    {
        colon = std::strchr(startline, ':');

        if(colon == NULL || (colon - startline -1) <= 0)
        {
            break;
        }

        header.first.assign(startline, colon - startline);

        endline = std::strstr(startline, "\r\n");

        if(endline != NULL)
        {
            endline_length = 2;
        }
        else
        {
            endline = std::strchr(startline, '\n');

            if(endline != NULL)
            {
                endline_length = 1;
            }
            else
            {
                break;
            }
        }

        header.second.assign(colon + 1, endline);
        _frame.addOrReplaceHeader(header);
        startline = endline + endline_length;
    }

    _startline_pointer = headers_endline + headers_endline_length;

    return true;
}

bool ns_parseBody(Stomp::Frame& _frame, const char* _frame_data, int _size, const char*& _startline_pointer)
{
    if(_startline_pointer <= _frame_data || (_startline_pointer - _frame_data) >= _size )
    {
        return false;
    }

    const char* null_terminator = std::strchr(_startline_pointer, '\0');

    if(null_terminator + 1 - _frame_data != _size)
    {
        return false;
    }

    _frame.setBody(_startline_pointer, null_terminator - _startline_pointer);

    _startline_pointer = null_terminator + 1;

    return true;
}

void ns_parseFrame(Stomp::Frame& _frame, const char* _frame_data, int _size)
{
    if(_size <= 0 || _frame_data[_size] != '\0')
    {
        throw StompException("Data invalid to Stomp::Frame(_frame_data)");
    }

    const char* startline_pointer = _frame_data;

    if(!ns_parseCommand(_frame, _frame_data, _size, startline_pointer))
    {
        throw StompException("Command invalid to Stomp::Frame(_frame_data)");
    }

    if(!ns_parseHeaders(_frame, _frame_data, _size, startline_pointer))
    {
        throw StompException("Header invalid to Stomp::Frame(_frame_data)");
    }

    if(!ns_parseBody(_frame, _frame_data, _size, startline_pointer))
    {
        throw StompException("Body invalid to Stomp::Frame(_frame_data)");
    }
}

std::string ns_getCommandMessage(Stomp::Command& _command)
{
    switch(_command)
    {
        // client commands
    case Stomp::CONNECT:
    {
        return "CONNECT";
    }
    case Stomp::SEND:
    {
        return "SEND";
    }
    case Stomp::SUBSCRIBE:
    {
        return "SUBSCRIBE";
    }
    case Stomp::UNSUBSCRIBE:
    {
        return "UNSUBSCRIBE";
    }
    case Stomp::BEGIN:
    {
        return "BEGIN";
    }
    case Stomp::COMMIT:
    {
        return "COMMIT";
    }
    case Stomp::ABORT:
    {
        return "ABORT";
    }
    case Stomp::ACK:
    {
        return "ACK";
    }
    case Stomp::DISCONNECT:
    {
        return "DISCONNECT";
    }
    // server commands
    case Stomp::CONNECTED:
    {
        return "CONNECTED";
        break;
    }
    case Stomp::MESSAGE:
    {
        return "MESSAGE";
        break;
    }
    case Stomp::RECEIPT:
    {
        return "RECEIPT";
        break;
    }
    case Stomp::ERROR:
    {
        return "ERROR";
        break;
    }
    default:
    {
        return "UNKNOWN";
        break;
    }
    }
}
}

Stomp::Frame::Frame(std::string _frame_data)
{
    ns_parseFrame(*this, _frame_data.c_str(), _frame_data.size());
}

Stomp::Frame::Frame(const char* _frame_data, int _size)
{
    ns_parseFrame(*this, _frame_data, _size);
}

Stomp::Frame::Frame() : m_command(Stomp::UNKNOWN)
{
}

Stomp::Frame::~Frame()
{
}

const Stomp::Command& Stomp::Frame::getCommand() const
{
    return m_command;
}

void Stomp::Frame::setCommand(const Stomp::Command& _command)
{
    m_command = _command;
}

const Stomp::Body& Stomp::Frame::getBody() const
{
    return m_body;
}

void Stomp::Frame::setBody(const Stomp::Body& _body)
{
    m_body << _body.rdbuf();
}

void Stomp::Frame::setBody(const char* _body_data, int _size)
{
    if(_size <= 0 || _body_data[_size] != '\0')
    {
        return;
    }

    m_body.write(_body_data, _size);
}

void Stomp::Frame::addOrReplaceHeader(Stomp::Header _header)
{
    Stomp::HeaderIterator found = m_headers.find(_header.first);

    if(found != m_headers.end())
    {
        found->second = _header.second;
    }
    else
    {
        m_headers.insert(_header);
    }
}
std::string Stomp::Frame::getHeaderValueFor(std::string _header_name)
{
    Stomp::HeaderIterator found = m_headers.find(_header_name);

    if(found != m_headers.end())
    {
        return found->second;
    }

    return "";
}

std::string Stomp::Frame::getFrameData()
{
    std::stringstream frame_data;

    frame_data << ns_getCommandMessage(m_command) << '\n';

    Stomp::HeaderIterator it;

    for(it = m_headers.begin(); it != m_headers.end(); ++it)
    {
        frame_data << it->first << ':' << it->second << '\n';
    }

    frame_data << '\n';
    frame_data << m_body.str();
    frame_data << '\0';

    return frame_data.str();
}
