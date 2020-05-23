#include <gu/game_utils.h>
#include <audio/audio.h>
#include <asset_manager/asset.h>
#include <files/FileWatcher.h>
#include <audio/WavLoader.h>

#include "rendering/room/RoomScreen.h"
#include "multiplayer/io/web/WebSocket.h"
#include "multiplayer/io/web/WebSocketServer.h"
#include "multiplayer/io/MultiplayerIO.h"
#include "multiplayer/session/MultiplayerClientSession.h"
#include "multiplayer/session/MultiplayerServerSession.h"
#include "ImGuiStyle.h"
#include "rendering/Palette.h"
#include "rendering/sprites/MegaSpriteSheet.h"

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
    std::mutex assetToReloadMutex;
    std::string assetToReload;

    #ifdef linux
    FileWatcher watcher;
    watcher.addDirectoryToWatch("assets", true);

    watcher.onChange = [&] (auto path)
    {
        assetToReloadMutex.lock();
        assetToReload = path;
        assetToReloadMutex.unlock();
    };
    watcher.startWatchingAsync();
    #endif

    MegaSpriteSheet spriteSheet;

    AssetManager::addAssetLoader<TileSet>(".tileset.ase", [](auto path) {

        return new TileSet(path.c_str());
    });
    AssetManager::addAssetLoader<aseprite::Sprite>(".ase", [&](auto path) {

        auto sprite = new aseprite::Sprite;
        aseprite::Loader(path.c_str(), *sprite);
        spriteSheet.add(*sprite);
        return sprite;
    });
    AssetManager::addAssetLoader<std::string>(".frag|.vert", [](auto path) {

        return new std::string(File::readString(path.c_str()));
    });
    AssetManager::addAssetLoader<Palette>(".gpl", [](auto path) {

        return new Palette(path.c_str());
    });
    AssetManager::addAssetLoader<au::Sound>(".wav", [](auto path) {

        auto sound = new au::Sound;
        au::WavLoader(path.c_str(), *sound);
        return sound;
    });

    bool server = false;
    int serverPort = 0;
    if (argc == 3 && strcmp(argv[1], "--server") == 0)
    {
        server = true;
        serverPort = atoi(argv[2]);
    }
    std::function<void()> afterInit;
    MultiplayerSession *mpSession;

    #ifndef EMSCRIPTEN
    if (server)
    {
        auto ss = new MultiplayerServerSession(new WebSocketServer(serverPort));
        mpSession = ss;

        afterInit = [=] {
            ss->setLevel(Level::testLevel());
            ss->join("mikey maws");
        };
    }
    #endif

    if (!server)
    {
        SharedSocket ws = SharedSocket(new WebSocket(argc > 1 ? std::string(argv[1]) : "ws://192.168.2.5:55555"));
        mpSession = new MultiplayerClientSession(ws);

        ws->onOpen = [&]() {
            #ifdef EMSCRIPTEN
            mpSession->join(prompt("Enter your name"));
            #else
            mpSession->join("Dolan duk");
            #endif
        };
        ws->onConnectionFailed = []() {
            std::cout << "connection failed :C" << std::endl;

            #ifdef EMSCRIPTEN
            EM_ASM(
                alert("Websocket connection failed");
            );
            #endif
        };
        afterInit = [=] () mutable {
            ws->open();
        };
    }

    gu::Config config;
    config.width = 1900;
    config.height = 900;
    config.title = "dibidabidab";
    config.vsync = false;
    config.samples = 0;
    config.printOpenGLMessages = false;
    config.printOpenGLErrors = false;
    if (!gu::init(config))
        return -1;
    au::init();

    setImGuiStyle();

    std::cout << "Running game with\n - GL_VERSION: " << glGetString(GL_VERSION) << "\n";
    std::cout << " - GL_RENDERER: " << glGetString(GL_RENDERER) << "\n";

    std::vector<std::string> audioDevices;
    if (!au::getAvailableDevices(audioDevices, NULL))
        throw gu_err("could not get audio devices");

    std::cout << "Available audio devices:\n";
    for (auto &dev : audioDevices)
        std::cout << " - " << dev << std::endl;

    AssetManager::load("assets");

    Screen *roomScreen = NULL;

    mpSession->onNewLevel = [&](Level *lvl)
    {
        std::cout << "New level!\n";
        delete roomScreen;
        gu::setScreen(NULL);

        lvl->onPlayerEnteredRoom = [&](Room *room, int playerId)
        {
            if (!mpSession->getLocalPlayer() || playerId != mpSession->getLocalPlayer()->id)
                return;
            delete roomScreen;
            std::cout << "Local player entered room. Show RoomScreen\n";
            roomScreen = new RoomScreen(room, &spriteSheet, mpSession->isServer());
            gu::setScreen(roomScreen);
        };
    };

    gu::beforeRender = [&](double deltaTime) {
        mpSession->update(KeyInput::pressed(GLFW_KEY_KP_SUBTRACT) ? deltaTime * .1 : deltaTime);
        if (KeyInput::justPressed(GLFW_KEY_F11))
            gu::fullscreen = !gu::fullscreen;

        if (KeyInput::justPressed(GLFW_KEY_R))
            AssetManager::load("assets");

        assetToReloadMutex.lock();
        if (!assetToReload.empty())
            AssetManager::loadFile(assetToReload, "assets/");
        assetToReload.clear();
        assetToReloadMutex.unlock();
    };

    mpSession->onJoinRequestDeclined = [&](auto reason) {
        #ifdef EMSCRIPTEN
        alertJS(reason.c_str());
        #endif
        mpSession->join(prompt("Try again. Enter your name"));
    };
    afterInit();

    gu::run();
    delete mpSession;


    #ifdef EMSCRIPTEN
    return 0;
    #else
    quick_exit(0);
    #endif
}
