
#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include "room/Room.h"

class Level
{
    double time = 0;
    Room *rooms = NULL;
    int nrOfRooms = 0;

    bool updating = false;

    friend void to_json(json& j, const Level& lvl);
    friend void from_json(const json& j, Level& lvl);

  public:

    Level() = default;

    std::function<void(Room *, int playerId)> onPlayerEnteredRoom;

    int getNrOfRooms() const { return nrOfRooms; }

    Room &getRoom(int i);

    const Room &getRoom(int i) const;

    double getTime() const { return time; }

    bool isUpdating() const { return updating; }

    /**
     * Updates the level and it's Rooms.
     *
     * @param deltaTime Time passed since previous update
     */
    void update(double deltaTime);

    static Level *testLevel();

    ~Level();

};

void to_json(json& j, const Level& lvl);

void from_json(const json& j, Level& lvl);

#endif
