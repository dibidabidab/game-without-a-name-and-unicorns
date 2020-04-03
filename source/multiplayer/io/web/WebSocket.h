
#ifndef GAME_WEBSOCKET_H
#define GAME_WEBSOCKET_H

#include <utils/gu_error.h>

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

#include "../Socket.h"

/**
 * Wrapper class for websockets on different platforms.
 */
class WebSocket : public Socket
{

  public:
    /**
     * Create a WebSocket.
     * call .open() to actually connect
     */
    WebSocket(const std::string &url);

    void open() override;

    void send(const char *data, unsigned int length) override;

    void close() override;

    bool isOpened() const override { return opened; }

    bool isClosed() const override { return closed; }

  private:

    bool opened = false, closed = false;

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
