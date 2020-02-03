
#ifndef GAME_TERRAINCOLLISIONDETECTOR_H
#define GAME_TERRAINCOLLISIONDETECTOR_H

class Room;
struct AABB;

struct TerrainCollisions
{
    bool
        floor     = false,
        slopeDown = false,
        slopeUp   = false,
        ceiling   = false,
        leftWall  = false,
        rightWall = false;
};

class TerrainCollisionDetector
{
    Room *room;

  public:
    explicit TerrainCollisionDetector(Room *room) : room(room) {};

    TerrainCollisions detect(AABB aabb);

  private:

    bool ceilingIntersection(AABB aabb);
    bool slopeDownIntersection(AABB aabb);
    bool slopeUpIntersection(AABB aabb);
    bool floorIntersection(AABB aabb);
    bool leftWallIntersection(AABB aabb);
    bool rightWallIntersection(AABB aabb);

};


#endif
