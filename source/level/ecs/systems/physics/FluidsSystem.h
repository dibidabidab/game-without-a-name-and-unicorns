
#ifndef GAME_FLUIDSSYSTEM_H
#define GAME_FLUIDSSYSTEM_H

#include "../EntitySystem.h"
#include "../../../Level.h"
#include "../../components/physics/Physics.h"

class FluidsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

    Room *room = NULL;

  protected:
    void update(double deltaTime, Room *room) override;

    void spawnFluidSplash(const asset<au::Sound>&, const Physics &, const AABB &, const Fluid &);

};


#endif
