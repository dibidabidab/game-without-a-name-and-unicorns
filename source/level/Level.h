
#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <json.hpp>
#include "room/Room.h"

/**
 * A level contains one or more Rooms.
 */
class Level
{
    double time = 0;
    std::vector<Room *> rooms;

    bool updating = false;
    float updateAccumulator = 0;
    constexpr static int
        MAX_UPDATES_PER_SEC = 100,
        MIN_UPDATES_PER_SEC = 30,
        MAX_UPDATES_PER_FRAME = 1;

    friend void to_json(json& j, const Level& lvl);
    friend void from_json(const json& j, Level& lvl);

  public:

    const std::string loadedFromFile;

    std::string spawnRoom;

    Level() = default;

    Level(const char *loadFromFile);

    std::function<void(Room *, int playerId)> onPlayerEnteredRoom, onPlayerLeftRoom;
    std::function<void(Room *)> beforeRoomDeletion = [](auto) {};

    int getNrOfRooms() const { return rooms.size(); }

    Room &getRoom(int i);

    const Room &getRoom(int i) const;

    Room *getRoomByName(const char *);

    const Room *getRoomByName(const char *) const;

    void deleteRoom(int i);

    void createRoom(int width, int height, const Room *duplicate=NULL);

    double getTime() const { return time; }

    bool isUpdating() const { return updating; }

    void initialize();

    /**
     * Updates the level and it's Rooms.
     *
     * @param deltaTime Time passed since previous update
     */
    void update(double deltaTime);

    static Level *testLevel();

    void save(const char *path) const;

    ~Level();

};

void to_json(json& j, const Level& lvl);

void from_json(const json& j, Level& lvl);

#endif
