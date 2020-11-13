#include <asset_manager/asset.h>
#include <utils/aseprite/AsepriteLoader.h>
#include <game/dibidab.h>

#include "rendering/Palette.h"
#include "rendering/sprites/MegaSpriteSheet.h"
#include "game/Game.h"
#include "rendering/GameScreen.h"
#include "tiled_room/TiledRoom.h"

void addAssetLoaders()
{
    // more loaders are added in dibidab::init()

    AssetManager::addAssetLoader<TileSet>(".tileset.ase", [](auto path) {

        return new TileSet(path.c_str());
    });
    AssetManager::addAssetLoader<aseprite::Sprite>(".ase", [&](auto path) {

        auto sprite = new aseprite::Sprite;
        aseprite::Loader(path.c_str(), *sprite);
        Game::spriteSheet.add(*sprite);
        return sprite;
    });
    AssetManager::addAssetLoader<Palette>(".gpl", [](auto path) {

        return new Palette(path.c_str());
    });
}

void initLuaStuff()
{
    auto &env = luau::getLuaState();

    env["getSettings"] = [] {
        json j = dibidab::settings;
        j.merge_patch(Game::settings);
        return j;
    };
    env["saveSettings"] = [] (const json &j) {
        Game::settings = j;
        dibidab::settings = j;
        Game::saveSettings();
    };

    // colors:
    auto colorTable = sol::table::create(env.lua_state());
    Palette &palette = Game::palettes->effects.at(0).lightLevels[0].get();
    int i = 0;
    for (auto &[name, color] : palette.colors)
        colorTable[name] = i++;
    env["colors"] = colorTable;

    // sprite function: // todo: maybe this should not be in main.cpp
    env["playAsepriteTag"] = [] (AsepriteView &view, const char *tagName, sol::optional<bool> unpause) -> float {
        return view.playTag(tagName, unpause.value_or(false));
    };
    env["asepriteTagFrames"] = [] (AsepriteView &view, const char *tagName) {
        assert(view.sprite.isSet());
        auto &tag = view.sprite->getTagByName(tagName);
        return std::make_tuple(tag.from, tag.to);
    };
}

int main(int argc, char *argv[])
{
    Game::loadSettings();

    addAssetLoaders();

    Level::customRoomLoader = [] (const json &j) {
        auto *room = new TiledRoom;
        room->fromJson(j);
        return room;
    };

    dibidab::init(argc, argv);

    File::createDir("./saves"); // todo, see trello
    gu::setScreen(new GameScreen);

    initLuaStuff();

    Game::uiScreenManager->openScreen(asset<luau::Script>("scripts/ui_screens/StartupScreen"));

    dibidab::run();

    Game::saveSettings();

    #ifdef EMSCRIPTEN
    return 0;
    #else
    quick_exit(0);
    #endif
}
