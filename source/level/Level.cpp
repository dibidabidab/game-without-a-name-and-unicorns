
#include <files/file.h>
#include <gu/game_utils.h>
#include <utils/gu_error.h>
#include <cstring>

#include "Level.h"
#include "ecs/entity_templates/PlayerEntity.h"

void Level::initialize()
{
    assert(rooms != NULL);
    for (int i = 0; i < nrOfRooms; i++)
    {
        rooms[i].registerEntityTemplate<PlayerEntity>();
        rooms[i].initialize(this, i);
    }
}

void Level::update(double deltaTime)
{
    gu::profiler::Zone levelUpdateZone("level update");
    assert(rooms != NULL);
    updating = true;
    time += deltaTime;

    for (int i = 0; i < nrOfRooms; i++)
        rooms[i].update(deltaTime);
    updating = false;
}

#define DEFAULT_LEVEL_PATH "assets/default_level.lvl"

Level::~Level()
{
    std::vector<unsigned char> data;
    json j;
    to_json(j, *this);
    json::to_cbor(j, data);
    File::writeBinary(DEFAULT_LEVEL_PATH, data);

    delete[] rooms;
}

Room &Level::getRoom(int i)
{
    if (i < 0 || i >= nrOfRooms) throw gu_err("index out of bounds");
    return rooms[i];
}

const Room &Level::getRoom(int i) const
{
    if (i < 0 || i >= nrOfRooms) throw gu_err("index out of bounds");
    return rooms[i];
}

void to_json(json &j, const Level &lvl)
{
    j = json::array();
    for (int i = 0; i < lvl.nrOfRooms; i++)
        j.push_back(lvl.getRoom(i));
}

void from_json(const json &j, Level &lvl)
{
    lvl.nrOfRooms = j.size();
    lvl.rooms = new Room[lvl.nrOfRooms];
    for (int i = 0; i < lvl.nrOfRooms; i++)
    {
        from_json(j[i], lvl.rooms[i]);
        lvl.rooms[i].level = &lvl;
    }
}

Level *Level::testLevel()
{
    auto lvl = new Level();

    if (File::exists(DEFAULT_LEVEL_PATH))
    {
        auto data = File::readBinary(DEFAULT_LEVEL_PATH);
        json j = json::from_cbor(data);
        from_json(j, *lvl);
    }
    else
    {
        lvl->nrOfRooms = 1;
        lvl->rooms = new Room[1] { Room(ivec2(20, 18)) };
        TileMap &map = lvl->getRoom(0).getMap();
        for (int x = 0; x < 20; x++)
            map.setTile(x, 0, Tile::full);
    }
    return lvl;
}
