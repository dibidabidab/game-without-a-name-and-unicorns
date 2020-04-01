
#ifndef GAME_WEBSOCKET_H
#define GAME_WEBSOCKET_H

#ifdef EMSCRIPTEN

#include <emscripten/websocket.h>

struct EmscriptenCallbackFunctions;

#else

#include <asio.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client>               websockclient;
typedef websocketpp::server<websocketpp::config::asio>                      websockserver;
typedef std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> websockcon;

class WebSocketServer;

#endif

#include <string>
#include <functional>

/**
 * Wrapper class for websockets on different platforms.
 */
class WebSocket
{

  public:
    /**
     * URL this socket is connected to.
     * Will be empty if this socket was created by a server instead of a client.
     */
    const std::string url;

    std::function<void(WebSocket *)>
            onOpen              = [](auto){},
            onConnectionFailed  = [](auto){},
            onClose             = [](auto){};
    std::function<void(WebSocket *, const char *data, int size)>
            onMessage           = [](auto, auto, auto){};

    /**
     * Create a WebSocket.
     * call .open() to actually connect
     */
    WebSocket(const std::string &url);

    /**
     * Try to open the connection
     * @return success
     */
    void open();

    /**
     * send data over the socket
     */
    void send(const char *data, unsigned int length);

    /**
     * close the socket
     */
    void close();

    /**
     * Returns true if the websocket is ready to use
     */
    bool isOpened() const { return opened; }

  private:

    bool opened = false;

    #ifdef EMSCRIPTEN

    // ID of the WebSocket in Emscripten:
    EMSCRIPTEN_WEBSOCKET_T emSockId;

    // make sure the callback functions can access private members:
    friend struct EmscriptenCallbackFunctions;

    #else

    friend WebSocketServer;

    websockcon connectionCreatedByServer;

    /**
     * Wrap an already existing WebSocket created by a websocketpp server.
     */
    WebSocket(websockcon connection);


    //----------

    websockclient *client = NULL;
    websockclient::connection_ptr clientConnection;

  public:
    ~WebSocket();

    #endif

};


#endif
