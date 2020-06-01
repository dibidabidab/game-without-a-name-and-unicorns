
#ifndef GAME_PHYSICSSYSTEM_H
#define GAME_PHYSICSSYSTEM_H


#include "../EntitySystem.h"
#include "../../../Level.h"
#include "../../components/physics/Physics.h"
#include "../../components/physics/PolyPlatform.h"
#include "../../components/Polyline.h"

namespace
{
    enum Move
    {
        up, down, left, right, none
    };
}

/**
 * Responsible for moving an entity based on:
 *  - velocity
 *  - gravity
 *  - terrain collisions
 *  - collisions with StaticColliders
 *  - distance constraints
 *  - poly platform movement
 */
class PhysicsSystem : public EntitySystem
{
    Room *room = NULL;

    using EntitySystem::EntitySystem;
  protected:
    void update(double deltaTime, Room *room) override;

  private:

    void moveEntitiesOnPolyPlatform(const PolyPlatform &, const ivec2 &);

    void preventFallingThroughPolyPlatform(Physics &, AABB &);

    void updateDistanceConstraint(AABB &aabb, const DistanceConstraint &constraint);

    void repositionAfterCollision(const AABB &staticAABB, AABB &dynAABB, entt::entity dynEntity);

    TerrainCollisionDetector *collisionDetector;

    /**
     * Updates the velocity of a body.
     */
    void updateVelocity(Physics &physics, double deltaTime);

    /**
     * Updates the position of a body using its velocity
     */
    void updatePosition(Physics &physics, AABB &body, double deltaTime);

    template <typename vec>
    void moveBody(Physics &physics, AABB &body, vec &pixelsToMove);

    /**
     * Will try to do the move, returns true if success
     */
    bool tryMove(Physics &physics, AABB &body, Move toDo);

    /**
     * Checks if a move is possible.
     * Sometimes a move is possible but requires another move to be done as well,
     * therefore `moveToDo` will be changed to that other move (or Move::none)
     */
    bool canDoMove(Physics &p, AABB &aabb, Move &moveToDo, Move prevMove);

    /**
     * Executes the move (without checking if the move is possible to do)
     */
    void doMove(Physics &p, AABB &body, Move move);

    void updateTerrainCollisions(Physics &p, AABB &body);

};

#endif
