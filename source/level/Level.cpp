
#include <files/file.h>
#include <fstream>

#include "Level.h"
#include "ecs/systems/physics/PhysicsSystem.h"
#include "ecs/systems/PlatformerMovementSystem.h"
#include "ecs/systems/networking/NetworkingSystem.h"

#define DEFAULT_ROOM_PATH "assets/default_room.room"

Level::Level()
{
    if (File::exists(DEFAULT_ROOM_PATH))
    {
        auto data = File::readBinary(DEFAULT_ROOM_PATH);
        currentRoom = new Room(&data[0]);
    }
    else currentRoom = new Room(20, 18);

    // todo: remove this, this is just so that the player doesn't fall into the void
    currentRoom->setTile(1, 0, Tile::full);
    currentRoom->setTile(2, 0, Tile::full);

    systems.push_back(new PlatformerMovementSystem());
    systems.push_back(new PhysicsSystem());
    systems.push_back(new NetworkingSystem());

    for (auto sys : systems) sys->init(this);
}

Level::~Level()
{
    std::vector<uint8> data;
    currentRoom->toBinary(data);
    File::writeBinary(DEFAULT_ROOM_PATH, data);
}

void Level::update(double deltaTime)
{
    time += deltaTime;

    for (auto sys : systems)
    {
        if (sys->disabled) continue;

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
