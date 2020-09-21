
#ifndef GAME_PHYSICSSYSTEM_H
#define GAME_PHYSICSSYSTEM_H


#include "../EntitySystem.h"
#include "../../../level/Level.h"
#include "../../../generated/Physics.hpp"
#include "../../../generated/PolyPlatform.hpp"
#include "../../../generated/Polyline.hpp"

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
    void init(EntityEngine *room) override;

    void update(double deltaTime, EntityEngine *) override;

  private:

    void moveEntitiesOnPolyPlatform(const PolyPlatform &, const ivec2 &);

    void preventFallingThroughPolyPlatform(Physics &, AABB &);

    void onDistanceConstraintCreated(entt::registry &, entt::entity);
    void updateDistanceConstraint(AABB &, const DistanceConstraint &);

    void repositionAfterCollision(const AABB &staticAABB, AABB &dynAABB, entt::entity dynEntity);

    TerrainCollisionDetector *collisionDetector;

    /**
     * Updates the velocity of a body.
     */
    void updateVelocity(Physics &, AABB &, double deltaTime);

    /**
     * Updates the position of a body using its velocity
     */
    void updatePosition(Physics &, AABB &, double deltaTime);

    void updateWind(Physics &, AABB &, double deltaTime);

    template <typename vec>
    void moveBody(Physics &, AABB &, vec &pixelsToMove);

    /**
     * Will try to do the move, returns true if success
     */
    bool tryMove(Physics &, AABB &, Move);

    /**
     * Checks if a move is possible.
     * Sometimes a move is possible but requires another move to be done as well,
     * therefore `moveToDo` will be changed to that other move (or Move::none)
     */
    bool canDoMove(Physics &, AABB &, Move &moveToDo, Move prevMove);

    /**
     * Executes the move (without checking if the move is possible to do)
     */
    void doMove(Physics &, AABB &, Move);

    void updateTerrainCollisions(Physics &, AABB &);

};

#endif
