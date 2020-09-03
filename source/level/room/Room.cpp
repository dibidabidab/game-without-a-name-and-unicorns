
#include <gu/profiler.h>
#include "Room.h"

#include "../../ecs/systems/physics/PhysicsSystem.h"
#include "../../ecs/systems/PlatformerMovementSystem.h"
#include "../../ecs/systems/networking/NetworkingSystem.h"
#include "../../ecs/systems/PlayerControlSystem.h"
#include "../../ecs/systems/physics/VerletPhysicsSystem.h"
#include "../../ecs/systems/body_parts/LegsSystem.h"
#include "../../ecs/systems/graphics/SpriteSystem.h"
#include "../../ecs/systems/body_parts/LimbJointSystem.h"
#include "../../ecs/systems/body_parts/ArmsSystem.h"
#include "../../ecs/systems/combat/BowWeaponSystem.h"
#include "../../ecs/systems/ChildrenSystem.h"
#include "../../ecs/systems/body_parts/HeadsSystem.h"
#include "../../ecs/systems/combat/ArrowSystem.h"
#include "../../ecs/systems/AudioSystem.h"
#include "../../ecs/systems/physics/WavesSystem.h"

#include "../../ecs/systems/RainbowSystem.h"
#include "../../ecs/systems/SpawningSystem.h"
#include "../../ecs/systems/combat/DamageSystem.h"
#include "../../ecs/systems/graphics/SpriteSlicerSystem.h"
#include "../../ecs/systems/graphics/LightSystem.h"
#include "../../ecs/systems/physics/FluidsSystem.h"
#include "../../ecs/systems/TransRoomerSystem.h"
#include "../../ecs/components/Saving.h"
#include "../../ecs/systems/LuaScriptsSystem.h"

Room::Room(ivec2 size)
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

void Room::initialize(Level *lvl)
{
    assert(lvl != NULL);
    assert(tileMap != NULL);

    level = lvl;

    entities.on_destroy<Persistent>().connect<&Room::tryToSaveRevivableEntity>(this);

    addSystem(new SpawningSystem("(de)spawning")); // SPAWN ENTITIES FIRST, so they get a chance to be updated before being rendered
    addSystem(new DamageSystem());
    addSystem(new TransRoomerSystem("transroomer"));
    addSystem(new ChildrenSystem("children"));
    addSystem(new PlayerControlSystem("player control"));
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
    addSystem(new AudioSystem("audio"));
    addSystem(new LuaScriptsSystem("lua functions"));

    EntityEngine::initialize();

    loadPersistentEntities();
}

void Room::update(double deltaTime)
{
    gu::profiler::Zone roomZone("room " + std::to_string(getIndexInLevel()));

    EntityEngine::update(deltaTime);

    tileMap->tileUpdatesPrevUpdate = tileMap->tileUpdatesSinceLastUpdate;
    tileMap->tileUpdatesSinceLastUpdate.clear();
    tileMap->wind.update(deltaTime);
}

Room::~Room()
{
    delete tileMap;
}

TileMap &Room::getMap() const
{
    if (!tileMap) throw gu_err("Room doesn't have a TileMap");
    return *tileMap;
}

void to_json(json &j, const Room &r)
{

    std::vector<char> tileMapBinary;
    r.getMap().toBinary(tileMapBinary);
    std::string tileMapBase64 = base64::encode(&tileMapBinary[0], tileMapBinary.size());
    j = json{
            {"name", r.name},
            {"lightLevel", r.baseLightLevel},
            {"position", r.positionInLevel},
            {"width", r.getMap().width},
            {"height", r.getMap().height},
            {"tileMapBase64", tileMapBase64}
    };
    j["entities"] = r.revivableEntitiesToSave;
    r.entities.view<const Persistent>().each([&](auto e, const Persistent &persistent) {

        j["entities"].push_back(json::object());
        r.persistentEntityToJson(e, persistent, j["entities"].back());
    });
}

void from_json(const json &j, Room &r)
{
    r.name = j.at("name");
    r.baseLightLevel = j.at("lightLevel");
    r.persistentEntitiesToLoad = j.at("entities");
    r.positionInLevel = j.at("position");
    r.tileMap = new TileMap(ivec2(j.at("width"), j.at("height")));
    std::string tileMapBase64 = j.at("tileMapBase64");
    auto tileMapBinary = base64::decode(&tileMapBase64[0], tileMapBase64.size());
    r.tileMap->fromBinary(&tileMapBinary[0], tileMapBinary.size());
}

void Room::loadPersistentEntities()
{
    for (json &jsonEntity : persistentEntitiesToLoad)
    {
        auto e = entities.create();
        assert(entities.valid(e));
        try
        {
            auto &p = entities.assign<Persistent>(e);
            p.data = jsonEntity.at("data");
            if (jsonEntity.contains("position"))
            {
                auto &aabb = entities.assign<AABB>(e);
                aabb.center = jsonEntity.at("position");
            }

            for (auto &[componentName, componentJson] : jsonEntity.at("components").items())
            {
                auto utils = ComponentUtils::getFor(componentName);
                if (!utils)
                    throw gu_err("Tried to load " + componentName + "-component from json. That Component-type does not exist!");

                utils->setJsonComponentWithKeys(componentJson, e, entities);
            }

            std::string applyTemplate = jsonEntity.at("template");
            if (!applyTemplate.empty())
                getTemplate(applyTemplate).createComponents(e, true);

        } catch (std::exception &exc)
        {
            std::cerr << "Error while loading entity from JSON: \n" << exc.what() << std::endl;
            entities.destroy(e);
        }
    }
    persistentEntitiesToLoad.clear();
}

void Room::resize(int moveLeftBorder, int moveRightBorder, int moveTopBorder, int moveBottomBorder)
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

int Room::nrOfPersistentEntities() const
{
    return persistentEntitiesToLoad.is_array() ? persistentEntitiesToLoad.size() : 0;
}

void Room::persistentEntityToJson(entt::entity e, const Persistent &persistent, json &j) const
{
    j["template"] = persistent.applyTemplateOnLoad;
    j["data"] = persistent.data;
    if (persistent.saveFinalPosition)
    {
        const AABB *aabb = entities.try_get<AABB>(e);
        if (aabb)
            j["position"] = aabb->center;
    }
    else if (persistent.saveSpawnPosition)
        j["position"] = persistent.spawnPosition;

    json &componentsJson = j["components"] = json::object();

    for (auto &componentTypeName : persistent.saveAllComponents ? ComponentUtils::getAllComponentTypeNames() : persistent.saveComponents)
    {
        auto utils = ComponentUtils::getFor(componentTypeName);
        if (utils->entityHasComponent(e, entities))
            utils->getJsonComponentWithKeys(componentsJson[componentTypeName], e, entities);
    }
}

void Room::tryToSaveRevivableEntity(entt::registry &, entt::entity entity)
{
    Persistent &p = entities.get<Persistent>(entity);
    if (!p.revive)
        return;

    revivableEntitiesToSave.push_back(json::object());
    persistentEntityToJson(entity, p, revivableEntitiesToSave.back());
}

void Room::initializeLuaEnvironment()
{
    EntityEngine::initializeLuaEnvironment();

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
