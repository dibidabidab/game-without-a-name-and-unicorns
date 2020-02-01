
#include <files/file.h>
#include <fstream>

#include "Level.h"

#define DEFAULT_ROOM_PATH "default_room.room"

Level::Level()
{
    if (File::exists(DEFAULT_ROOM_PATH))
    {
        auto data = File::readBinary(DEFAULT_ROOM_PATH);
        currentRoom = new Room(&data[0]);
    }
    else currentRoom = new Room(20, 18);
}

Level::~Level()
{
    std::vector<uint8> data;
    currentRoom->toBinary(data);
    File::writeBinary(DEFAULT_ROOM_PATH, data);
}
