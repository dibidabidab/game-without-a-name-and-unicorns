#include <gu/game_utils.h>
#include <utils/math_utils.h>

#include "RoomScreen.h"
#include "multiplayer/io/web/WebSocket.h"
#include "multiplayer/io/web/WebSocketServer.h"
#include "multiplayer/io/MultiplayerIO.h"
#include "multiplayer/session/MultiplayerClientSession.h"
#include "multiplayer/session/MultiplayerServerSession.h"
#include "ImGuiStyle.h"

#ifdef EMSCRIPTEN
EM_JS(const char *, promptJS, (const char *text), {

    var input = prompt(UTF8ToString(text));
    var lengthBytes = lengthBytesUTF8(input) + 1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(input, stringOnWasmHeap, lengthBytes);
    return stringOnWasmHeap;
});
EM_JS(void, alertJS, (const char *text), {

    alert(UTF8ToString(text));
});
EM_JS(const char *, urlHash, (), {

    var input = prompt(UTF8ToString(text));
    var lengthBytes = lengthBytesUTF8(input) + 1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(input, stringOnWasmHeap, lengthBytes);
    return stringOnWasmHeap;
});
#endif

std::string prompt(std::string text)
{
#ifdef EMSCRIPTEN
    const char *input = promptJS(text.c_str());
    std::string inputStr(input);
    free((char *)input);
    return inputStr;
#else
    std::string input;
    std::cout << text << ": ";
    std::getline(std::cin, input);
    return input;
#endif
}

int main(int argc, char *argv[])
{
#ifndef EMSCRIPTEN
    MultiplayerServerSession mpSession(new WebSocketServer(55555));

#else
    SharedSocket ws = SharedSocket(new WebSocket(argc > 1 ? std::string(argv[1]) : "ws://192.168.2.5:25565"));
    MultiplayerClientSession mpSession(ws);

    ws->onOpen = [&]() {
        mpSession.join(prompt("Enter your name"));
    };
    ws->onConnectionFailed = []() {
        std::cout << "connection failed :C\n";

        #ifdef EMSCRIPTEN
        EM_ASM(
            alert("Websocket connection failed");
        );
        #endif
    };
#endif

    {
        gu::Config config;
        config.width = 1900;
        config.height = 900;
        config.title = "dibidabidab";
        config.vsync = false;
        config.samples = 0;
        config.printOpenGLMessages = false;
        if (!gu::init(config))
            return -1;

        setImGuiStyle();

        std::cout << "Running game with OpenGL version: " << glGetString(GL_VERSION) << "\n";

        Screen *roomScreen = NULL;

        mpSession.onNewLevel = [&](Level *lvl)
        {
            std::cout << "New level!\n";
            delete roomScreen;
            gu::setScreen(NULL);

            lvl->onPlayerEnteredRoom = [&](Room *room, int playerId)
            {
                if (!mpSession.getLocalPlayer() || playerId != mpSession.getLocalPlayer()->id)
                    return;
                delete roomScreen;
                std::cout << "Player entered room. Show RoomScreen\n";
                roomScreen = new RoomScreen(room);
                gu::setScreen(roomScreen);
            };
        };

        gu::beforeRender = [&](double deltaTime) {
            mpSession.update(deltaTime);
            if (KeyInput::justPressed(GLFW_KEY_F11))
                gu::fullscreen = !gu::fullscreen;
        };

        mpSession.onJoinRequestDeclined = [&](auto reason) {
            #ifdef EMSCRIPTEN
            alertJS(reason.c_str());
            #endif
            mpSession.join(prompt("Try again. Enter your name"));
        };

        #ifndef EMSCRIPTEN
        mpSession.setLevel(Level::testLevel());
        mpSession.join("mikey maws");
        #else
        ws->open();
        #endif

        gu::run();
    }

    #ifdef EMSCRIPTEN
    return 0;
    #else
    quick_exit(0);
    #endif
}
