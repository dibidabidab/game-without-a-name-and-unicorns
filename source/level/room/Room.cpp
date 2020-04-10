
#include <gu/profiler.h>
#include "Room.h"

#include "../ecs/systems/physics/PhysicsSystem.h"
#include "../ecs/systems/PlatformerMovementSystem.h"
#include "../ecs/systems/networking/NetworkingSystem.h"

void Room::update(double deltaTime)
{
    if (!initialized)
    {
        systems.push_back(new PlatformerMovementSystem("pltf movmnt"));
        systems.push_back(new PhysicsSystem("physics"));
        systems.push_back(new NetworkingSystem("networking"));

        for (auto sys : systems) sys->init(this);
        initialized = true;
    }

    for (auto sys : systems)
    {
        if (sys->disabled) continue;
        gu::profiler::Zone sysZone(sys->name);

        if (sys->updateFrequency == .0) sys->update(deltaTime, this);
        else
        {
            sys->updateAccumulator += deltaTime;
            while (sys->updateAccumulator > sys->updateFrequency)
            {
                sys->update(sys->updateFrequency, this);
                sys->updateAccumulator -= sys->updateFrequency;
            }
        }
    }
}

Room::~Room()
{
    for (auto sys : systems)
        delete sys;
}

TileMap &Room::getMap() const
{
    if (!tileMap) throw gu_err("Room doesn't have a TileMap");
    return *tileMap;
}

Room::Room(ivec2 size)
{
    tileMap = new TileMap(size);
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
