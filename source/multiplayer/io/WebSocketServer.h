
#ifndef EMSCRIPTEN
#ifndef GAME_WEBSOCKETSERVER_H
#define GAME_WEBSOCKETSERVER_H

#include <asio.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include "WebSocket.h"

typedef websocketpp::server<websocketpp::config::asio> websockserver;
typedef std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> websockcon;

class WebSocketServer
{
    websockserver server;
    std::map<websockcon, WebSocket *> connectionToWebSocket;

  public:
    explicit WebSocketServer(int port);

    const int port;

    std::function<void(WebSocket *)> onNewSocket;

    void start();

    void stop();

    ~WebSocketServer();

};


#endif
#endif
