
#include <gu/profiler.h>
#include "Room.h"

#include "ecs/systems/physics/PhysicsSystem.h"
#include "ecs/systems/PlatformerMovementSystem.h"
#include "ecs/systems/networking/NetworkingSystem.h"
#include "ecs/systems/PlayerControlSystem.h"
#include "ecs/systems/physics/VerletPhysicsSystem.h"
#include "ecs/systems/body_parts/LegsSystem.h"
#include "ecs/systems/graphics/SpriteSystem.h"
#include "ecs/systems/body_parts/LimbJointSystem.h"
#include "ecs/systems/body_parts/ArmsSystem.h"
#include "ecs/systems/combat/BowWeaponSystem.h"
#include "ecs/systems/ChildrenSystem.h"
#include "ecs/systems/body_parts/HeadsSystem.h"
#include "ecs/systems/combat/ArrowSystem.h"
#include "ecs/systems/AudioSystem.h"
#include "ecs/systems/physics/WavesSystem.h"

#include "ecs/entity_templates/LuaEntityTemplate.h"
#include "ecs/systems/RainbowSystem.h"
#include "ecs/systems/SpawningSystem.h"
#include "ecs/systems/combat/DamageSystem.h"
#include "ecs/systems/graphics/SpriteSlicerSystem.h"
#include "ecs/systems/graphics/LightSystem.h"
#include "ecs/systems/physics/FluidsSystem.h"
#include "ecs/systems/TransRoomerSystem.h"
#include "ecs/components/Saving.h"
#include "ecs/systems/LuaScriptsSystem.h"

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
    assert(!initialized);
    assert(lvl != NULL);
    assert(tileMap != NULL);

    level = lvl;

    for (auto &el : AssetManager::getLoadedAssetsForType<LuaEntityScript>())
        registerLuaEntityTemplate(el.second->shortPath.c_str());

    systems.push_front(new LuaScriptsSystem("lua functions"));
    systems.push_front(new AudioSystem("audio"));
    systems.push_front(new LimbJointSystem("knee/elbow joints"));
    systems.push_front(new LightSystem("lights"));
    systems.push_front(new SpriteSystem("(animated) sprites"));
    systems.push_front(new SpriteSlicerSystem("sprite slicer"));
    systems.push_front(new LegsSystem("legs"));
    systems.push_front(new ArmsSystem("arms"));
    systems.push_front(new HeadsSystem("heads"));
    systems.push_front(new WavesSystem("polyline waves"));
    systems.push_front(new VerletPhysicsSystem("verlet physics"));
    systems.push_front(new FluidsSystem("fluids"));
    systems.push_front(new PhysicsSystem("physics"));
    systems.push_front(new RainbowSystem("rainbows"));
    systems.push_front(new ArrowSystem("bow arrows"));
    systems.push_front(new BowWeaponSystem("bow weapons"));
    systems.push_front(new PlatformerMovementSystem("pltf movmnt"));
    systems.push_front(new PlayerControlSystem("player control"));
    systems.push_front(new ChildrenSystem("children"));
    systems.push_front(new TransRoomerSystem("transroomer"));
    systems.push_front(new DamageSystem());
    systems.push_front(new SpawningSystem("(de)spawning")); // SPAWN ENTITIES FIRST, so they get a chance to be updated before being rendered

    for (auto sys : systems) sys->init(this);

    loadPersistentEntities();

    initialized = true;
}

void Room::update(double deltaTime)
{
    if (!initialized)
        throw gu_err("Cannot update non-initialized Room!");

    gu::profiler::Zone roomZone("room " + std::to_string(getIndexInLevel()));

    for (auto sys : systems)
    {
        if (!sys->enabled) continue;
        gu::profiler::Zone sysZone(sys->name);

        if (sys->updateFrequency == .0) sys->update(deltaTime, this);
        else
        {
            float customDeltaTime = 1. / sys->updateFrequency;
            sys->updateAccumulator += deltaTime;
            while (sys->updateAccumulator > customDeltaTime)
            {
                sys->update(customDeltaTime, this);
                sys->updateAccumulator -= customDeltaTime;
            }
        }
    }
    tileMap->tileUpdatesPrevUpdate = tileMap->tileUpdatesSinceLastUpdate;
    tileMap->tileUpdatesSinceLastUpdate.clear();
    tileMap->wind.update(deltaTime);
}

Room::~Room()
{
    for (auto sys : systems)
        delete sys;
    for (auto &entry : entityTemplates)
        delete entry.second;
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
    j["entities"] = json::array();
    r.entities.view<const Persistent>().each([&](auto e, const Persistent &persistent) {

        j["entities"].push_back(json::object());
        json &entityJson = j["entities"].back();

        entityJson["template"] = persistent.applyTemplateOnLoad;
        entityJson["data"] = persistent.data;
        if (persistent.savePosition)
        {
            const AABB *aabb = r.entities.try_get<AABB>(e);
            if (aabb)
                entityJson["position"] = aabb->center;
        }

        json &componentsJson = entityJson["components"] = json::object();

        for (auto &componentTypeName : persistent.saveAllComponents ? ComponentUtils::getAllComponentTypeNames() : persistent.saveComponents)
        {
            auto utils = ComponentUtils::getFor(componentTypeName);
            if (utils->entityHasComponent(e, r.entities))
                utils->getJsonComponentWithKeys(componentsJson[componentTypeName], e, r.entities);
        }
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

void Room::addSystem(EntitySystem *sys)
{
    assert(!initialized);
    systems.push_back(sys);
}

EntityTemplate &Room::getTemplate(std::string name)
{
    try {
        return getTemplate(hashStringCrossPlatform(name));
    }
    catch (_gu_err &)
    {
        throw gu_err("No EntityTemplate named " + name + " found");
    }
}

EntityTemplate &Room::getTemplate(int templateHash)
{
    auto t = entityTemplates[templateHash];
    if (!t)
        throw gu_err("No EntityTemplate found for hash " + std::to_string(templateHash));
    return *t;
}

const std::vector<std::string> &Room::getTemplateNames() const
{
    return entityTemplateNames;
}

entt::entity Room::getChildByName(entt::entity parent, const char *childName)
{
    const Parent *p = entities.try_get<Parent>(parent);
    if (!p)
        return entt::null;
    auto it = p->childrenByName.find(childName);
    if (it == p->childrenByName.end())
        return entt::null;
    return it->second;
}

void Room::registerLuaEntityTemplate(const char *assetPath)
{
    auto name = splitString(assetPath, "/").back();

    addEntityTemplate(name, new LuaEntityTemplate(assetPath, name.c_str(), this));
}

void Room::addEntityTemplate(const std::string &name, EntityTemplate *t)
{
    int hash = hashStringCrossPlatform(name);

    bool replace = entityTemplates[hash] != NULL;

    delete entityTemplates[hash];
    auto et = entityTemplates[hash] = t;
    et->room = this;
    et->templateHash = hash;

    if (!replace)
        entityTemplateNames.push_back(name);
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
