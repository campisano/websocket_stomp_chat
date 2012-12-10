#include "Websocket.h"

#include <iostream>
#include <libwebsockets.h>

struct WebsocketUserData
{
    struct libwebsocket* wsi;
    struct libwebsocket_context* context;
};

namespace
{
WebsocketUsers* ns_users;
unsigned int* ns_user_counter;
ISocketHandler* ns_socket_handler;

WebsocketUsersIterator ns_find(struct libwebsocket* _wsi)
{
    WebsocketUsersIterator it;

    for(it = ns_users->begin(); it != ns_users->end(); ++it)
    {
        if(it->second.wsi == _wsi)
        {
            break;
        }
    }

    return it;
}

static int ns_callback_http(struct libwebsocket_context* _this_context,
                            struct libwebsocket* _wsi,
                            enum libwebsocket_callback_reasons _reason,
                            void* _user,
                            void* _in,
                            size_t _len)
{
    return 0;
}

static int ns_callback_websocket(struct libwebsocket_context* _this_context,
                                 struct libwebsocket* _wsi,
                                 enum libwebsocket_callback_reasons _reason,
                                 void* _user,
                                 void* _in,
                                 size_t _len)
{
    switch (_reason)
    {
    case LWS_CALLBACK_ESTABLISHED:
    {
        std::cout << "connection established: user " << (*ns_user_counter) << std::endl;
        WebsocketUserData user_data = { _wsi, _this_context};
        ns_users->insert(WebsocketUser(*ns_user_counter, user_data));
        ns_socket_handler->connect(*ns_user_counter);
        ++(*ns_user_counter);
        break;
    }
    case LWS_CALLBACK_RECEIVE:
    {
        std::cout << "______Received______" << std::endl;
        std::cout << std::string((char *) _in, _len) << std::endl;
        std::cout << "____________________" << std::endl;

        WebsocketUsersIterator it = ns_find(_wsi);

        if(it != ns_users->end())
        {
            ns_socket_handler->receive(it->first, (char *) _in, _len);
        }
        else
        {
            std::string message = "error, can't find you, client!";
            std::cerr << message << std::endl;
            unsigned char* buffer = new unsigned char[LWS_SEND_BUFFER_PRE_PADDING + message.size() + LWS_SEND_BUFFER_POST_PADDING];

            for(size_t i = 0; i < message.size(); ++i)
            {
                buffer[LWS_SEND_BUFFER_PRE_PADDING + i ] = message[i];
            }

            libwebsocket_write(_wsi, &buffer[LWS_SEND_BUFFER_PRE_PADDING], message.size(), LWS_WRITE_CLOSE);
            delete[] buffer;
        }
        break;
    }
    case LWS_CALLBACK_CLOSED:
    {

        WebsocketUsersIterator it = ns_find(_wsi);

        if(it != ns_users->end())
        {
            std::cout << "connection closed: user " << it->first << std::endl;
            ns_socket_handler->disconnect(it->first);
            ns_users->erase(it);
        }
        else
        {
            std::cerr << "connection closed: user " << ((void*) _wsi) << "is not in the user list!" << std::endl;
        }
        break;
    }
    default:
    {
        break;
    }
    }

    return 0;
}

static struct libwebsocket_protocols protocols[] =
{
    // first protocol must always be HTTP handler
    {
        "http-only",                // name
        ns_callback_http,           // callback
        0                           // per_session_data_size
    },
    //TODO wrong place to specify "v11.stomp"
    {
        "v11.stomp",                // protocol name - very important!
        ns_callback_websocket,      // callback
        0                           // we don't use any per session data
    },
    {
        NULL, NULL, 0               // End of list
    }
};
}

Websocket::Websocket(unsigned int _port) : m_keep_running(false), m_port(_port), m_user_counter(0)
{
    ns_users = &m_users;
    ns_user_counter = &m_user_counter;
}

Websocket::~Websocket()
{
}

bool Websocket::run(ISocketHandler& _socket_handler)
{
    ns_socket_handler = &_socket_handler;
    const char* interface = NULL;
    // we're not using ssl
    const char* cert_path = NULL;
    const char* key_path = NULL;
    // no special options
    int opts = 0;

    // create libwebsocket context representing this server
    struct libwebsocket_context* context = libwebsocket_create_context(m_port, interface, protocols,
                                           libwebsocket_internal_extensions,
                                           cert_path, key_path, -1, -1, opts, NULL);

    if (context == NULL)
    {
        std::cerr << "libwebsocket init failed" << std::endl;

        return false;
    }

    std::cout << "starting server..." << std::endl;

    m_keep_running = true;

    // infinite loop, to end this server send SIGTERM. (CTRL+C)
    while(m_keep_running)
    {
        libwebsocket_service(context, 50);
        // libwebsocket_service will process all waiting events with their
        // callback functions and then wait 50 ms.
        // (this is a single threaded webserver and this will keep our server
        // from generating load while there are not requests to process)
    }

    libwebsocket_context_destroy(context);

    return true;
}

void Websocket::send(unsigned int _id, std::string _data)
{
    WebsocketUsersIterator it = m_users.find(_id);

    std::cout << "_____Sending...____" << std::endl;
    std::cout << _data << std::endl;
    std::cout << "____________________" << std::endl;

    if(it == ns_users->end())
    {
        std::cerr << "Websocket::send() can't find user: " << _id << std::endl;

        return;
    }

    size_t size = _data.size();
    unsigned char* buffer = new unsigned char[LWS_SEND_BUFFER_PRE_PADDING + size + LWS_SEND_BUFFER_POST_PADDING];

    for(size_t i = 0; i < size; ++i)
    {
        buffer[LWS_SEND_BUFFER_PRE_PADDING + i ] = _data[i];
    }

    libwebsocket_write(it->second.wsi, &buffer[LWS_SEND_BUFFER_PRE_PADDING], size, LWS_WRITE_TEXT);
    delete[] buffer;
}

void Websocket::disconnect(unsigned int _id)
{
    WebsocketUsersIterator it = m_users.find(_id);

    if(it == ns_users->end())
    {
        std::cerr << "Websocket::disconnect() can't find user: " << _id << std::endl;

        return;
    }

    libwebsocket_close_and_free_session( it->second.context, it->second.wsi, LWS_CLOSE_STATUS_NOSTATUS);
}
