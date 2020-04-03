#include <gu/game_utils.h>

#include "LevelScreen.h"
#include "multiplayer/io/web/WebSocket.h"
#include "multiplayer/io/web/WebSocketServer.h"
#include "multiplayer/io/MultiplayerIO.h"

REFLECTABLE_STRUCT(
        ChatMsg,

        FIELD(std::string, text)
)
END_REFLECTABLE_STRUCT(ChatMsg)

#ifndef EMSCRIPTEN
#include <asio.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>


WebSocketServer *websocksvr()
{
    auto *server = new WebSocketServer(25565);

    server->onNewSocket = [](auto sock) {

        std::cout << "wowie " << sock->url << "\n";

        auto io = new MultiplayerIO(sock, MultiplayerIO::PacketHandlingMode::IMMEDIATELY_ON_SOCKET_THREAD);

        io->addJsonPacketHandler<ChatMsg>([=](auto msg) {
            std::cout << "Server received: " << msg->text << '\n';
            delete msg;
        });
        io->printTypes();

//        sock->onMessage = [](Socket *sock, const char *data, int size) {
//            std::cout << "received " << size << " bytes from " << sock->url << '\n';
//
//            sock->send(data, size);
//        };
        sock->onClose = [=]() {
            std::cout << "awwwww bye\n";
            delete io;
        };
    };

    server->start();
    return server;
}



#else
#include <emscripten/websocket.h>

#endif

int main()
{
    double time = 0;
#ifndef EMSCRIPTEN
    auto svr = websocksvr();

    gu::beforeRender = [&](double deltaTime) {

        time += deltaTime;
        if (time > 10)
            svr->stop();
    };

//#else
    SharedSocket ws = SharedSocket(new WebSocket("ws://192.168.2.5:25565"));

    auto io = new MultiplayerIO(ws);

    io->addJsonPacketHandler<ChatMsg>([=](auto msg) {
        std::cout << "Client received: " << msg->text << '\n';
        delete msg;
    });
    io->printTypes();

    std::cout << io << '\n';

    ws->onOpen = [=]() {
        std::cout << "wowie socket is open\n";

        ChatMsg toSend {"first pizza"};
        std::cout << io << '\n';
        io->send(toSend);

//        std::vector<char> data{'a', 'b', 'c'};
//        ws->send(&data[0], 3);
    };
//    ws->onMessage = [](auto, const char *data, int size) {
//        std::cout << "received " << size << " bytes\n";
//    };
    ws->onClose = [=]() {
        std::cout << "socket is closed :C\n";

        #ifdef EMSCRIPTEN
        EM_ASM(
                alert("Websocket was closed");
        );
        #endif
    };
    ws->onConnectionFailed = []() {
        std::cout << "connection failed :C\n";

        #ifdef EMSCRIPTEN
        EM_ASM(
                alert("Websocket connection failed");
        );
        #endif
    };

    ws->open();

    gu::beforeRender = [&](double deltaTime) {

        time += deltaTime;
        if (time > 10)
            svr->stop();

        if (io)
        {
            if (ws->isClosed())
            {
                delete io;
                io = NULL;
                ws = NULL;
                return;
            }

            io->handlePackets();
            ChatMsg toSend {"hi"};
//            io->send(toSend);
        }
    };

#endif

    gu::Config config;
    config.width = 1900;
    config.height = 900;
    config.title = "My game";
    config.showFPSInTitleBar = true; // note: this option will hide the default title.
    config.vsync = true;
    config.samples = 0;
    if (!gu::init(config))
        return -1;

    std::cout << "Running game with OpenGL version: " << glGetString(GL_VERSION) << "\n";

    LevelScreen scr;

    gu::setScreen(&scr);

    gu::run();

    return 0;
}
