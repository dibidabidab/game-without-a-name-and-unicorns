
#ifndef GAME_BOWWEAPONSYSTEM_H
#define GAME_BOWWEAPONSYSTEM_H

#include <ecs/systems/EntitySystem.h>

class BowWeaponSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, EntityEngine *room) override;

};


#endif
