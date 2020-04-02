
#ifndef EMSCRIPTEN

#include "WebSocketServer.h"



WebSocketServer::WebSocketServer(int port) : port(port)
{
    server.set_access_channels(websocketpp::log::alevel::all);
    server.clear_access_channels(websocketpp::log::alevel::all);
    server.set_error_channels(websocketpp::log::elevel::all);
    server.set_reuse_addr(true);

    server.set_open_handler([&](websocketpp::connection_hdl hdl) {
        auto conn = server.get_con_from_hdl(hdl);

        std::cout << conn->get_raw_socket().remote_endpoint() << " OPENED SOCKET TO SERVER\n";

        WebSocket *sock = new WebSocket(conn);

        connectionToWebSocket[conn] = sock;
        onNewSocket(sock);
    });
    server.set_message_handler([&](websocketpp::connection_hdl hdl, websockserver::message_ptr msg) {
        auto conn = server.get_con_from_hdl(hdl);

        WebSocket *sock = connectionToWebSocket[conn];

        sock->onMessage(msg->get_payload().data(), msg->get_payload().size());
    });
    server.set_close_handler([&](websocketpp::connection_hdl hdl) {

        auto conn = server.get_con_from_hdl(hdl);

        WebSocket *sock = connectionToWebSocket[conn];

        sock->onClose();

        std::cout << sock->url << " SOCKET WAS CLOSED\n";

        connectionToWebSocket.erase(conn);
        delete sock;

        std::cout << "(" << connectionToWebSocket.size() << " WebSocket(s) active)\n";
    });
}

WebSocketServer::~WebSocketServer()
{
    if (!server.stopped())
        std::cerr <<
                "WebSocketServer-object on port " << std::to_string(port)
                << " destroyed before actual server was stopped. Did the object go out of scope?";

    for (auto &sock : connectionToWebSocket)
        delete sock.second;
}

void WebSocketServer::start()
{
    server.init_asio();
    server.listen(port);
    server.start_accept();
    std::thread([&] {
        server.run();
    }).detach();
}

void WebSocketServer::stop()
{
    server.stop();
}

#endif
