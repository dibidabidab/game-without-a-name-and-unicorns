
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
    double gameTime = 0;     // time that can be paused

    Room *rooms = NULL;
    int nrOfRooms = 0;

    bool updating = false;  // Is The level currently updating any of the rooms

    friend void to_json(json& j, const Level& lvl);
    friend void from_json(const json& j, Level& lvl);

  public:
    bool paused = false;    // Is the game paused

    Level() = default;

    std::function<void(Room *, int playerId)> onPlayerEnteredRoom;

    int getNrOfRooms() const { return nrOfRooms; }

    Room &getRoom(int i);

    const Room &getRoom(int i) const;

    double getTime() const { return time; }
    double getGameTime() const { return gameTime; }

    bool isUpdating() const { return updating; }

    void initialize();

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
