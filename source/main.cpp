
#include "gu/game_utils.h"

#include "LevelScreen.h"
#include "multiplayer/io/WebSocket.h"
#include "multiplayer/io/WebSocketServer.h"

#ifndef EMSCRIPTEN
#include <asio.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>



void websocksvr()
{
    WebSocketServer *server = new WebSocketServer(25565);

    server->onNewSocket = [](auto sock) {
        std::cout << "wowie " << sock->url << "\n";
        sock->onMessage = [](WebSocket *sock, const char *data, int size) {
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

    ws->onOpen = [&](WebSocket *ws) {
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

int main() {

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
    ws.onClose = [&](auto) {
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
