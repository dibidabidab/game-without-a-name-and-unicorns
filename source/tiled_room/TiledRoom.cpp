
#include "TiledRoom.h"
#include <utils/base64.h>

#include "../ecs/systems/physics/PhysicsSystem.h"
#include "../ecs/systems/PlatformerMovementSystem.h"
#include "../ecs/systems/physics/VerletPhysicsSystem.h"
#include "../ecs/systems/body_parts/LegsSystem.h"
#include "../ecs/systems/graphics/SpriteSystem.h"
#include "../ecs/systems/body_parts/LimbJointSystem.h"
#include "../ecs/systems/body_parts/ArmsSystem.h"
#include "../ecs/systems/combat/BowWeaponSystem.h"
#include "../ecs/systems/body_parts/HeadsSystem.h"
#include "../ecs/systems/combat/ArrowSystem.h"
#include "../ecs/systems/physics/WavesSystem.h"

#include "../ecs/systems/RainbowSystem.h"
#include "../ecs/systems/combat/DamageSystem.h"
#include "../ecs/systems/graphics/SpriteSlicerSystem.h"
#include "../ecs/systems/graphics/LightSystem.h"
#include "../ecs/systems/physics/FluidsSystem.h"
#include "../ecs/systems/TransRoomerSystem.h"

TiledRoom::TiledRoom(ivec2 size)
{
    tileMap = new TileMap(size);
    for (int x = 0; x < size.x; x++)
    {
        tileMap->setTile(x, 0, Tile::full, 0, false, false);
        tileMap->setTile(x, size.y - 1, Tile::full, 0, false, false);
    }
    for (int y = 0; y < size.y; y++)
    {
        tileMap->setTile(0, y, Tile::full, 0, false, false);
        tileMap->setTile(size.x - 1, y, Tile::full, 0, false, false);
    }
    tileMap->refreshOutlines();
}

void TiledRoom::initialize(Level *lvl)
{
    assert(tileMap != NULL);

    addSystem(new DamageSystem());
    addSystem(new TransRoomerSystem("transroomer"));
    addSystem(new PlatformerMovementSystem("pltf movmnt"));
    addSystem(new BowWeaponSystem("bow weapons"));
    addSystem(new ArrowSystem("bow arrows"));
    addSystem(new RainbowSystem("rainbows"));
    addSystem(new PhysicsSystem("physics"));
    addSystem(new FluidsSystem("fluids"));
    addSystem(new VerletPhysicsSystem("verlet physics"));
    addSystem(new WavesSystem("polyline waves"));
    addSystem(new HeadsSystem("heads"));
    addSystem(new ArmsSystem("arms"));
    addSystem(new LegsSystem("legs"));
    addSystem(new SpriteSlicerSystem("sprite slicer"));
    addSystem(new SpriteSystem("(animated) sprites"));
    addSystem(new LightSystem("lights"));
    addSystem(new LimbJointSystem("knee/elbow joints"));

    Room::initialize(lvl);
}

void TiledRoom::update(double deltaTime)
{
    Room::update(deltaTime);

    tileMap->tileUpdatesPrevUpdate = tileMap->tileUpdatesSinceLastUpdate;
    tileMap->tileUpdatesSinceLastUpdate.clear();
    tileMap->wind.update(deltaTime);
}

TiledRoom::~TiledRoom()
{
    delete tileMap;
}

TileMap &TiledRoom::getMap() const
{
    if (!tileMap) throw gu_err("Room doesn't have a TileMap");
    return *tileMap;
}

void TiledRoom::resize(int moveLeftBorder, int moveRightBorder, int moveTopBorder, int moveBottomBorder)
{
    TileMap *old = tileMap;
    tileMap = new TileMap(ivec2(
        old->width + moveLeftBorder + moveRightBorder,
        old->height + moveTopBorder + moveBottomBorder
    ));
    tileMap->copy(
        *old,
        moveLeftBorder < 0 ? -moveLeftBorder : 0,
        moveBottomBorder < 0 ? -moveBottomBorder : 0,

        moveLeftBorder > 0 ? moveLeftBorder : 0,
        moveBottomBorder > 0 ? moveBottomBorder : 0,

        old->width, old->height
    );
    delete old;

    positionInLevel.x = max<int>(0, positionInLevel.x - moveLeftBorder);
    positionInLevel.y = max<int>(0, positionInLevel.y - moveBottomBorder);
}

void TiledRoom::initializeLuaEnvironment()
{
    Room::initializeLuaEnvironment();

    luaEnvironment["getTile"] = [&](int x, int y) -> int {
        return int(getMap().getTile(x, y));
    };
    // todo: setTile() etc.
    luaEnvironment["getTileMaterial"] = [&](int x, int y) -> int {
        return int(getMap().getMaterial(x, y));
    };
    luaEnvironment["getLevelTime"] = [&]() -> double {
        return getLevel().getTime();
    };
    luaEnvironment["roomWidth"] = getMap().width;
    luaEnvironment["roomHeight"] = getMap().height;
}

vec3 TiledRoom::getPosition(entt::entity e) const
{
    return entities.has<AABB>(e) ? vec3(entities.get<AABB>(e).center, 0) : vec3(0);
}

void TiledRoom::setPosition(entt::entity e, const vec3 &v)
{
    entities.get_or_assign<AABB>(e).center = v;
}

void TiledRoom::toJson(json &j) const
{
    Room::toJson(j);
    std::vector<char> tileMapBinary;
    getMap().toBinary(tileMapBinary);
    std::string tileMapBase64 = base64::encode(&tileMapBinary[0], tileMapBinary.size());
    j["lightLevel"] = baseLightLevel;
    j["position"] = positionInLevel;
    j["width"] = getMap().width;
    j["height"] = getMap().height;
    j["tileMapBase64"] = tileMapBase64;
}

void TiledRoom::fromJson(const json &j)
{
    Room::fromJson(j);
    baseLightLevel = j.at("lightLevel");
    positionInLevel = j.at("position");
    tileMap = new TileMap(ivec2(j.at("width"), j.at("height")));
    std::string tileMapBase64 = j.at("tileMapBase64");
    auto tileMapBinary = base64::decode(&tileMapBase64[0], tileMapBase64.size());
    tileMap->fromBinary(&tileMapBinary[0], tileMapBinary.size());
}
