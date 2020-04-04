
#ifndef EMSCRIPTEN

#include "WebSocketServer.h"



WebSocketServer::WebSocketServer(int port) : SocketServer(port)
{
    server.set_access_channels(websocketpp::log::alevel::all);
    server.clear_access_channels(websocketpp::log::alevel::all);
    server.set_error_channels(websocketpp::log::elevel::all);
    server.set_reuse_addr(true);

    server.set_open_handler([&](websocketpp::connection_hdl hdl) {
        auto conn = server.get_con_from_hdl(hdl);

        std::cout << conn->get_raw_socket().remote_endpoint() << " OPENED SOCKET TO SERVER\n";

        SharedSocket sock = SharedSocket(new WebSocket(conn));

        mapMutex.lock();
        connectionToSocket[conn] = sock;
        mapMutex.unlock();
        onNewSocket(sock);
    });
    server.set_message_handler([&](websocketpp::connection_hdl hdl, websockserver::message_ptr msg) {
        auto conn = server.get_con_from_hdl(hdl);

        SharedSocket sock = connectionToSocket[conn];

        sock->onMessage(msg->get_payload().data(), msg->get_payload().size());
    });
    server.set_close_handler([&](websocketpp::connection_hdl hdl) {

        mapMutex.lock();
        auto conn = server.get_con_from_hdl(hdl);

        SharedSocket sock = connectionToSocket[conn];

        ((WebSocket *)sock.get())->closed = true;
        sock->onClose();

        std::cout << sock->url << " SOCKET WAS CLOSED\n";

        connectionToSocket.erase(conn);

        std::cout << "(" << connectionToSocket.size() << " WebSocket(s) active)\n";

        if (stopCalled && connectionToSocket.empty())
            server.stop();

        mapMutex.unlock();
    });
}

WebSocketServer::~WebSocketServer()
{
    if (!server.stopped())
        std::cerr <<
                "WebSocketServer-object on port " << std::to_string(port)
                << " destroyed before actual server was stopped. Did the object go out of scope?";
}

void WebSocketServer::start()
{
    server.init_asio();
    server.listen(port);
    server.start_accept();
    std::thread([&] {
        server.run();
        std::cout << "WebSocketServer stopped\n";
    }).detach();
}

void WebSocketServer::stop()
{
    if (stopCalled) return;
    mapMutex.lock();
    stopCalled = true;
    server.stop_listening();
    for (const auto& s : connectionToSocket)
        s.second->close();

    mapMutex.unlock();
}

#endif
