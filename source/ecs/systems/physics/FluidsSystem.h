
#ifndef GAME_FLUIDSSYSTEM_H
#define GAME_FLUIDSSYSTEM_H

#include <ecs/systems/EntitySystem.h>
#include "../../../generated/Physics.hpp"
#include "../../../generated/PolyPlatform.hpp"
#include "../../../generated/Polyline.hpp"

class FluidsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

    EntityEngine *room = NULL;

  protected:
    void update(double deltaTime, EntityEngine *room) override;

    void spawnFluidSplash(
            const asset<au::Sound>&,
            const Fluid &, entt::entity fluidE,
            const Physics &, const AABB &);

};


#endif
