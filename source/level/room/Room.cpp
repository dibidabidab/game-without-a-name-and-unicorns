
#include <gu/profiler.h>
#include "Room.h"

#include "../ecs/systems/physics/PhysicsSystem.h"
#include "../ecs/systems/PlatformerMovementSystem.h"
#include "../ecs/systems/networking/NetworkingSystem.h"
#include "../ecs/systems/PlayerControlSystem.h"

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
    systems.push_front(new PhysicsSystem("physics"));
    systems.push_front(new PlatformerMovementSystem("pltf movmnt"));
    systems.push_front(new PlayerControlSystem("player control"));

    for (auto sys : systems) sys->init(this);
    initialized = true;
}

void Room::update(double deltaTime)
{
    if (!initialized)
        throw gu_err("Cannot update non-initialized Room!");
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
    tileMap->tileUpdatesSinceLastUpdate.clear();
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

EntityTemplate *Room::getTemplate(std::string name)
{
    return getTemplate(hashStringCrossPlatform(name));
}

EntityTemplate *Room::getTemplate(int templateHash)
{
    auto t = entityTemplates[templateHash];
    if (!t)
        throw gu_err("No EntityTemplate found for hash " + std::to_string(templateHash));
    return t;
}