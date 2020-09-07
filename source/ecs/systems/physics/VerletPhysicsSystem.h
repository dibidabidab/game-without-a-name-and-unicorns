
#ifndef GAME_VERLETPHYSICSSYSTEM_H
#define GAME_VERLETPHYSICSSYSTEM_H

#include "../EntitySystem.h"
#include "../../../level/Level.h"
#include "../../components/physics/Physics.h"
#include "../../components/physics/VerletRope.h"

/**
 * based on https://github.com/dci05049/Verlet-Rope-Unity/blob/master/Tutorial%20Verlet%20Rope/Assets/Rope.cs
 */
class VerletPhysicsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:

    Room *room = NULL;

    void init(EntityEngine *engine) override;

    void update(double deltaTime, EntityEngine *) override;

    void onPotentionalChildRopeCreation(entt::registry &, entt::entity);

    void updateAttachToRope(const VerletRope &, AttachToRope &, AABB &);

    void updateRope(VerletRope &, AABB &, double deltaTime);

};


#endif
