
#include "gu/game_utils.h"

#include "LevelScreen.h"
#include "multiplayer/io/web/WebSocket.h"
#include "multiplayer/io/web/WebSocketServer.h"
#include "multiplayer/io/MultiplayerIO.h"

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
        sock->onMessage = [](Socket *sock, const char *data, int size) {
            std::cout << "received " << size << " bytes from " << sock->url << '\n';

            sock->send(data, size);
        };
        sock->onClose = [](auto sock){
            std::cout << "awwwww bye\n";
        };
    };

    server->start();
}

void websock()
{

    WebSocket *ws = new WebSocket("ws://192.168.2.5:25565");

    ws->onOpen = [&](Socket *ws) {
        std::cout << "wowie socket is open\n";

        std::vector<char> data{'a', 'b', 'c'};
        ws->send(&data[0], 3);
    };
    ws->onMessage = [](auto, const char *data, int size) {
        std::cout << "received " << size << " bytes\n";
    };
    ws->onClose = [](auto) {
        std::cout << "socket is closed :C\n";
    };
    ws->onConnectionFailed = [](auto) {
        std::cout << "connection failed :C\n";
    };

    ws->open();
}


#else
#include <emscripten/websocket.h>

#endif

struct Testttt {
    int b;
};

void addListener(MultiplayerIO &io)
{
    io.addPacketListener<Testttt>([](auto data, auto size) {
        assert(size == sizeof(int));
        return new Testttt{((int *) data)[0]};
    },
    [](Testttt *packet) {
        std::cout << packet->b << " whaaat \n";
    });
}

int main()
{

    std::cout << typeHashCrossPlatform<WebSocket>() << '\n';
    std::cout << typeHashCrossPlatform<WebSocket>() << '\n';
    std::cout << typeHashCrossPlatform<Level>() << '\n';
    std::cout << typeHashCrossPlatform<WebSocket>() << '\n';

    MultiplayerIO io;
    addListener(io);

    std::vector<char> data;
    data.resize(sizeof(uint32) + sizeof(int));
    uint32 type = typeHashCrossPlatform<Testttt>();
    memcpy(&data[0], &type, sizeof(uint32));
    int b = 4444;
    memcpy(&data[sizeof(uint32)], &b, sizeof(int));

    io.test(&data[0], data.size());

    return 0;

#ifndef EMSCRIPTEN
//    websocksvr();

    websock();

#else
    WebSocket ws("ws://192.168.2.5:25565");

    ws.onOpen = [&](auto) {
        std::cout << "wowie socket is open\n";

        std::vector<char> data{'a', 'b', 'c'};
        ws.send(&data[0], 3);
    };
    ws.onClose = [](auto) {
        std::cout << "socket is closed :C\n";

        EM_ASM(
            alert("Websocket was closed");
        );
    };
    ws.onConnectionFailed = [](auto) {
        std::cout << "connection failed :C\n";
        EM_ASM(
            alert("Websocket connection failed");
        );
    };

    ws.open();

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
