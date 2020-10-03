
#ifndef GAME_VERLETPHYSICSSYSTEM_H
#define GAME_VERLETPHYSICSSYSTEM_H

#include <ecs/systems/EntitySystem.h>
#include "../../../generated/Physics.hpp"
#include "../../../generated/VerletRope.hpp"
#include "../../../tiled_room/TiledRoom.h"

/**
 * based on https://github.com/dci05049/Verlet-Rope-Unity/blob/master/Tutorial%20Verlet%20Rope/Assets/Rope.cs
 */
class VerletPhysicsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:

    TiledRoom *room = NULL;

    void init(EntityEngine *engine) override;

    void update(double deltaTime, EntityEngine *) override;

    void onPotentionalChildRopeCreation(entt::registry &, entt::entity);

    void updateAttachToRope(const VerletRope &, AttachToRope &, AABB &);

    void updateRope(VerletRope &, AABB &, float deltaTime);

};


#endif
