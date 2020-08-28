
#include <files/file.h>
#include <gu/game_utils.h>
#include <utils/gu_error.h>
#include <cstring>
#include <zlib.h>

#include "Level.h"
#include "room/ecs/components/PlayerControlled.h"

void Level::initialize()
{
    int i = 0;
    for (auto &room : rooms)
    {
        room->roomI = i++;
        room->initialize(this);
    }
    initialized = true;
}

void Level::update(double deltaTime)
{
    gu::profiler::Zone levelUpdateZone("level update");
    updating = true;

    updateAccumulator += deltaTime;

    static float MIN_DELTA_TIME = 1. / MAX_UPDATES_PER_SEC;
    static float MAX_DELTA_TIME = 1. / MIN_UPDATES_PER_SEC;

    int updates = 0;

    while (updateAccumulator >= MIN_DELTA_TIME && updates++ < MAX_UPDATES_PER_FRAME)
    {
        float roomDeltaTime = min(updateAccumulator, MAX_DELTA_TIME);
        time += roomDeltaTime;

        {
            /*
             * This piece of code will update rooms that have a player in them.
             * Rooms are checked twice for a player in them, because a Player might have been spawned during another Room's update
             */

            std::vector<bool> skippedRoom(rooms.size(), true);

            for (int repeat = 0; repeat < 2; repeat++)
            {
                for (int i = 0; i < rooms.size(); i++)
                {
                    auto room = rooms[i];
                    if (!skippedRoom[i] || room->entities.empty<PlayerControlled>())
                    {
                        skippedRoom[i] = true;
                        continue;
                    }
                    skippedRoom[i] = false;
                    room->update(roomDeltaTime);
                }
            }
        }

        updateAccumulator -= roomDeltaTime;
    }

    if (updateAccumulator > 2.)
    {
        std::cerr << "Level::update() can't reach MIN_UPDATES_PER_SEC! Skipping " << updateAccumulator << "sec!" << std::endl;
        updateAccumulator = 0;
    }

    updating = false;
}

#define DEFAULT_LEVEL_PATH "assets/default_level.lvl"

Level::~Level()
{
    save(loadedFromFile.empty() ? DEFAULT_LEVEL_PATH : loadedFromFile.c_str());

    for (auto r : rooms)
        delete r;
}

Room &Level::getRoom(int i)
{
    if (i < 0 || i >= rooms.size()) throw gu_err("index out of bounds");
    return *rooms[i];
}

const Room &Level::getRoom(int i) const
{
    if (i < 0 || i >= rooms.size()) throw gu_err("index out of bounds");
    return *rooms[i];
}

void to_json(json &j, const Level &lvl)
{
    j = json::object({{"spawnRoom", lvl.spawnRoom}, {"rooms", json::array()}});
    for (auto room : lvl.rooms)
        j["rooms"].push_back(*room);
}

void from_json(const json &j, Level &lvl)
{
    lvl.spawnRoom = j.at("spawnRoom");
    const json &roomsJson = j.at("rooms");

    lvl.rooms.resize(roomsJson.size());
    for (int i = 0; i < roomsJson.size(); i++)
    {
        lvl.rooms[i] = new Room;
        from_json(roomsJson[i], *lvl.rooms[i]);
        lvl.rooms[i]->level = &lvl;
    }
}

Level *Level::testLevel()
{
    Level *lvl;

    if (File::exists(DEFAULT_LEVEL_PATH))
        lvl = new Level(DEFAULT_LEVEL_PATH);
    else
    {
        lvl = new Level;
        lvl->createRoom(48, 32);
    }
    return lvl;
}

void Level::deleteRoom(int i)
{
    assert(i < rooms.size());

    Room *old = rooms[i];
    beforeRoomDeletion(old);

    rooms[i] = rooms.back();
    rooms[i]->roomI = i;

    delete old;
    rooms.pop_back();
}

void Level::createRoom(int width, int height, const Room *duplicate)
{
    auto r = rooms.emplace_back(new Room(ivec2(width, height)));

    if (initialized)
    {
        r->roomI = rooms.size() - 1;
        r->initialize(this);
    }

    if (duplicate)
    {
        assert(width == duplicate->getMap().width);
        assert(height == duplicate->getMap().height);

        json j;
        to_json(j, *duplicate);
        from_json(j, *r);
        r->positionInLevel.x += width;
        r->positionInLevel.y += height;
    }
}

void Level::save(const char *path) const
{
    std::vector<unsigned char> data;
    json j;
    to_json(j, *this);
    json::to_cbor(j, data);


    std::vector<uint8> compressedData(12 + data.size() * 1.1);
    unsigned long compressedDataSize = compressedData.size();

    int zResult = compress(&compressedData[0], &compressedDataSize, &data[0], data.size());

    if (zResult != Z_OK)
        throw gu_err("Error while compressing level");

    // store original data size at end of buffer:
    compressedData.resize(compressedDataSize + sizeof(int));
    int *originalDataSize = (int *) &compressedData[compressedDataSize];
    *originalDataSize = int(data.size());

    File::writeBinary(path, compressedData);
}

Level::Level(const char *loadFromFile) : loadedFromFile(loadFromFile)
{
    try
    {
        auto compressedData = File::readBinary(loadFromFile);

        unsigned long compressedDataSize = compressedData.size() - sizeof(int);

        unsigned long originalDataSize = *((int *) &compressedData[compressedDataSize]);
        unsigned long originalDataSize_ = originalDataSize;

        std::vector<uint8> uncompressedData(originalDataSize);

        int zResult = uncompress(&uncompressedData[0], &originalDataSize, &compressedData[0], compressedDataSize);

        if (originalDataSize != originalDataSize_)
            throw gu_err("Length of uncompressed data does not match length of original data");

        if (zResult != Z_OK)
            throw gu_err("Error while UNcompressing");

        json j = json::from_cbor(uncompressedData);
        from_json(j, *this);
    }
    catch (std::exception &e)
    {
        throw gu_err("Failed to load level from " + loadedFromFile + ":\n" + e.what());
    }
}

Room *Level::getRoomByName(const char *n)
{
    for (auto &r : rooms)
        if (r->name == n)
            return r;
    return NULL;
}

const Room *Level::getRoomByName(const char *n) const
{
    return ((Level *) this)->getRoomByName(n);
}
