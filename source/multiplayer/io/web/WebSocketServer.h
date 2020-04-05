
#ifndef EMSCRIPTEN
#ifndef GAME_WEBSOCKETSERVER_H
#define GAME_WEBSOCKETSERVER_H

#include <asio.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include "WebSocket.h"
#include "../SocketServer.h"

typedef websocketpp::server<websocketpp::config::asio> websockserver;
typedef std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> websockcon;

class WebSocketServer : public SocketServer
{
    websockserver server;
    std::map<websockcon, SharedSocket> connectionToSocket;
    bool stopCalled = false;

    std::mutex mapMutex;

  public:
    explicit WebSocketServer(int port);

    void start() override;

    void stop() override;

    ~WebSocketServer();

};


#endif
#endif
