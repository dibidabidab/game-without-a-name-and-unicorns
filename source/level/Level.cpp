
#include <files/file.h>
#include <fstream>

#include "Level.h"
#include "ecs/systems/PhysicsSystem.h"

#define DEFAULT_ROOM_PATH "default_room.room"

Level::Level()
{
    if (File::exists(DEFAULT_ROOM_PATH))
    {
        auto data = File::readBinary(DEFAULT_ROOM_PATH);
        currentRoom = new Room(&data[0]);
    }
    else currentRoom = new Room(20, 18);

    systems.push_back(new PhysicsSystem());

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
