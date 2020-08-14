
#include <gu/profiler.h>
#include "Room.h"

#include "../ecs/systems/physics/PhysicsSystem.h"
#include "../ecs/systems/PlatformerMovementSystem.h"
#include "../ecs/systems/networking/NetworkingSystem.h"
#include "../ecs/systems/PlayerControlSystem.h"
#include "../ecs/systems/physics/VerletPhysicsSystem.h"
#include "../ecs/systems/body_parts/LegsSystem.h"
#include "../ecs/systems/graphics/SpriteSystem.h"
#include "../ecs/systems/body_parts/LimbJointSystem.h"
#include "../ecs/systems/body_parts/ArmsSystem.h"
#include "../ecs/systems/combat/BowWeaponSystem.h"
#include "../ecs/systems/ChildrenSystem.h"
#include "../ecs/systems/body_parts/HeadsSystem.h"
#include "../ecs/systems/combat/ArrowSystem.h"
#include "../ecs/systems/AudioSystem.h"
#include "../ecs/systems/physics/WavesSystem.h"

#include "../ecs/entity_templates/LuaEntityTemplate.h"
#include "../ecs/systems/RainbowSystem.h"
#include "../ecs/systems/SpawningSystem.h"
#include "../ecs/systems/combat/DamageSystem.h"
#include "../ecs/systems/graphics/SpriteSlicerSystem.h"
#include "../ecs/systems/graphics/LightSystem.h"
#include "../ecs/systems/physics/FluidsSystem.h"

Room::Room(ivec2 size)
{
    tileMap = new TileMap(size);
}

void Room::initialize(Level *lvl, int roomI_)
{
    assert(!initialized);
    assert(lvl != NULL);

    level = lvl;
    roomI = roomI_;

    for (auto &el : AssetManager::getLoadedAssetsForType<LuaEntityScript>())
        registerLuaEntityTemplate(el.second->shortPath.c_str());

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
    systems.push_front(new SpawningSystem("(de)spawning"));
    systems.push_front(new DamageSystem());

    for (auto sys : systems) sys->init(this);
    initialized = true;
}

void Room::update(double deltaTime)
{
    if (!initialized)
        throw gu_err("Cannot update non-initialized Room!");

    gu::profiler::Zone roomZone("room " + std::to_string(getIndexInLevel()));

    for (auto sys : systems)
    {
        if (sys->disabled) continue;
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
            {"width", r.getMap().width},
            {"height", r.getMap().height},
            {"tileMapBase64", tileMapBase64}
    };
}

void from_json(const json &j, Room &r)
{
    r.tileMap = new TileMap(ivec2(j.at("width"), j.at("height")));
    std::string tileMapBase64 = j.at("tileMapBase64");
    auto tileMapBinary = base64::decode(&tileMapBase64[0], tileMapBase64.size());
    r.tileMap->fromBinary(&tileMapBinary[0], tileMapBinary.size());
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

    addEntityTemplate(name, new LuaEntityTemplate(assetPath, this));
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
