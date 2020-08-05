#include <gu/game_utils.h>
#include <audio/audio.h>
#include <asset_manager/asset.h>
#include <files/FileWatcher.h>
#include <audio/WavLoader.h>
#include <audio/OggLoader.h>
#include <utils/code_editor/CodeEditor.h>
#include <utils/aseprite/AsepriteLoader.h>

#include "rendering/room/RoomScreen.h"
#include "multiplayer/io/web/WebSocket.h"
#include "multiplayer/io/web/WebSocketServer.h"
#include "multiplayer/io/MultiplayerIO.h"
#include "multiplayer/session/MultiplayerClientSession.h"
#include "multiplayer/session/MultiplayerServerSession.h"
#include "multiplayer/session/OfflineMultiplayerSession.h"
#include "ImGuiStyle.h"
#include "rendering/Palette.h"
#include "rendering/sprites/MegaSpriteSheet.h"
#include "level/ecs/entity_templates/LuaEntityTemplate.h"
#include "Game.h"

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
    Game::loadSettings();

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
    AssetManager::addAssetLoader<Texture>(".png|.jpg|.jpeg|.tga|.bmp|.psd|.gif|.hdr", [&](auto path) {

        return new Texture(Texture::fromImageFile(path.c_str()));
    });
    AssetManager::addAssetLoader<std::string>(".frag|.vert", [](auto path) {

        return new std::string(File::readString(path.c_str()));
    });
    AssetManager::addAssetLoader<json>(".json", [](auto path) {

        return new json(json::parse(File::readString(path.c_str())));
    });
    AssetManager::addAssetLoader<Palette>(".gpl", [](auto path) {

        return new Palette(path.c_str());
    });
    AssetManager::addAssetLoader<au::Sound>(".wav", [](auto path) {

        auto sound = new au::Sound;
        au::WavLoader(path.c_str(), *sound);
        return sound;
    });
    AssetManager::addAssetLoader<au::Sound>(".ogg", [](auto path) {

        auto sound = new au::Sound;
        au::OggLoader::load(path.c_str(), *sound);
        return sound;
    });
    AssetManager::addAssetLoader<LuaEntityScript>(".entity.lua", [](auto path) {

        return new LuaEntityScript(File::readString(path.c_str()));
    });

    gu::Config config;
    config.width = Game::settings.graphics.windowSize.x;
    config.height = Game::settings.graphics.windowSize.y;
    config.title = "dibidabidab";
    config.vsync = Game::settings.graphics.vsync;
    config.samples = 0;
    config.printOpenGLMessages = Game::settings.graphics.printOpenGLMessages;
    config.printOpenGLErrors = Game::settings.graphics.printOpenGLErrors;
    gu::fullscreen = Game::settings.graphics.fullscreen; // dont ask me why this is not in config
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

    if (!server && argc == 3 && strcmp(argv[1], "--join") == 0)
    {
        SharedSocket ws = SharedSocket(new WebSocket(std::string(argv[2])));
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
    else
    {
        // offline session:
        mpSession = new OfflineMultiplayerSession(Level::testLevel());
        afterInit = [=] {
            mpSession->join("poopoo");
        };
    }

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
        mpSession->update(KeyInput::pressed(GLFW_KEY_KP_SUBTRACT) ? deltaTime * .03 : deltaTime);
        if (KeyInput::justPressed(GLFW_KEY_F11))
            gu::fullscreen = !gu::fullscreen;

        if (KeyInput::justPressed(GLFW_KEY_R))
            AssetManager::load("assets");

        assetToReloadMutex.lock();
        if (!assetToReload.empty())
            AssetManager::loadFile(assetToReload, "assets/");
        assetToReload.clear();
        assetToReloadMutex.unlock();

        CodeEditor::drawGUI(
            ImGui::GetIO().Fonts->Fonts.back()  // default monospace font (added by setImGuiStyle())
        );

        if (!gu::fullscreen) // dont save fullscreen-resolution
            Game::settings.graphics.windowSize = ivec2(
                gu::width, gu::height
            );
        Game::settings.graphics.vsync = gu::config.vsync;
        Game::settings.graphics.fullscreen = gu::fullscreen;
    };

    mpSession->onJoinRequestDeclined = [&](auto reason) {
        #ifdef EMSCRIPTEN
        alertJS(reason.c_str());
        #endif
        mpSession->join(prompt("Try again. Enter your name"));
    };
    afterInit();

    gu::run();
    delete mpSession->getLevel(); // trigger tilemap save, todo: remove this
    delete mpSession;

    au::terminate();

    Game::saveSettings();

    #ifdef EMSCRIPTEN
    return 0;
    #else
    quick_exit(0);
    #endif
}
