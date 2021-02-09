#include <asset_manager/asset.h>
#include <utils/aseprite/AsepriteLoader.h>
#include <game/dibidab.h>
#include <graphics/3d/model.h>
#include <utils/json_model_loader.h>

#include "rendering/Palette.h"
#include "rendering/sprites/MegaSpriteSheet.h"
#include "game/Game.h"
#include "rendering/GameScreen.h"
#include "tiled_room/TiledRoom.h"

bool assetsLoaded = false;

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

    static VertBuffer *vertBuffer = NULL;

    AssetManager::addAssetLoader<std::vector<SharedModel>>(".ubj", [](auto path) {

        VertAttributes vertAttributes;
        vertAttributes.add(VertAttributes::POSITION);
        vertAttributes.add(VertAttributes::NORMAL);

        auto collection = new std::vector<SharedModel>(JsonModelLoader::fromUbjsonFile(path.c_str(), &vertAttributes));

        if (assetsLoaded && vertBuffer && !vertBuffer->isUploaded())
            vertBuffer->upload(true);   // upload before previous models get unloaded.

        if (!vertBuffer || vertBuffer->isUploaded())
            vertBuffer = VertBuffer::with(vertAttributes);

        for (auto &model : *collection)
            for (auto &part : model->parts)
            {
                if (part.mesh == NULL)
                    throw gu_err(model->name + " has part without a mesh!");

                if (part.mesh->vertBuffer)
                    continue; // this mesh was handled before

                for (int i = 0; i < part.mesh->nrOfVertices(); i++)
                    part.mesh->set<vec3>(part.mesh->get<vec3>(i, 0) * 16.f, i, 0);
                vertBuffer->add(part.mesh);
            }

        return collection;
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
    assetsLoaded = true;

    File::createDir("./saves"); // todo, see trello
    gu::setScreen(new GameScreen);

    auto onResize = gu::onResize += [] {
        ShaderDefinitions::defineInt("PIXEL_SCALING", Game::settings.graphics.pixelScaling);
    };
    gu::onResize();

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
