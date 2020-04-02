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


void websocksvr()
{
    auto *server = new WebSocketServer(25565);

    server->onNewSocket = [](auto sock) {

        std::cout << "wowie " << sock->url << "\n";

        auto io = new MultiplayerIO(sock);

        io->addJsonPacketHandler<ChatMsg>([=](auto msg) {
            std::cout << "received " << msg->text << '\n';

//            ChatMsg toSend {"from server to client"};
            io->send(*msg);
            delete msg;
        });
        io->printTypes();

//        sock->onMessage = [](Socket *sock, const char *data, int size) {
//            std::cout << "received " << size << " bytes from " << sock->url << '\n';
//
//            sock->send(data, size);
//        };
        sock->onClose = [=](){
            std::cout << "awwwww bye\n";

            delete io;
        };
    };

    server->start();
}



#else
#include <emscripten/websocket.h>

#endif

int main()
{
#ifndef EMSCRIPTEN
//    websocksvr();

//#else

    WebSocket *ws = new WebSocket("ws://192.168.2.5:25565");

    auto io = new MultiplayerIO(ws);

    io->addJsonPacketHandler<ChatMsg>([=](auto msg) {
//        std::cout << "received: " << msg->text << '\n';
        delete msg;

        ChatMsg toSend {"pizza" + std::to_string(io->nrOfPacketsReceived())};
        io->send(toSend);

        std::cout << "sent" << '\n';
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
        delete io;

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

#endif

//    gu::beforeRender = [&](double deltaTime) {
//        std::cout << "poepie\n";
//    };

    gu::Config config;
    config.width = 1900;
    config.height = 900;
    config.title = "My game";
    config.showFPSInTitleBar = true; // note: this option will hide the default title.
    config.vsync = false;
    config.samples = 0;
    if (!gu::init(config))
        return -1;

    std::cout << "Running game with OpenGL version: " << glGetString(GL_VERSION) << "\n";

    LevelScreen scr;

    gu::setScreen(&scr);

    gu::run();

    return 0;
}
