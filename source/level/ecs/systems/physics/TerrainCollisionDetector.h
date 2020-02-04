
#ifndef GAME_TERRAINCOLLISIONDETECTOR_H
#define GAME_TERRAINCOLLISIONDETECTOR_H

class Room;
struct AABB;

struct TerrainCollisions
{
    bool
        floor             = false,
        slopeDown         = false,
        slopeUp           = false,
        ceiling           = false,
        slopedCeilingDown = false,
        slopedCeilingUp   = false,
        leftWall          = false,
        rightWall         = false;
};

class TerrainCollisionDetector
{
    Room *room;

  public:
    explicit TerrainCollisionDetector(Room *room) : room(room) {};

    /**
     * Detects collisions with the terrain.
     */
    TerrainCollisions detect(const AABB &aabb, bool ignorePlatforms);

  private:

    bool ceilingIntersection(const AABB &aabb);
    bool slopedCeilingDownIntersection(const AABB &aabb);
    bool slopedCeilingUpIntersection(const AABB &aabb);
    bool slopeDownIntersection(const AABB &aabb);
    bool slopeUpIntersection(const AABB &aabb);
    bool floorIntersection(const AABB &aabb, bool ignorePlatforms);
    bool leftWallIntersection(const AABB &aabb);
    bool rightWallIntersection(const AABB &aabb);

};


#endif
