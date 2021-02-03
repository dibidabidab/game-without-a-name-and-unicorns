
#ifndef GAME_COMBATSYSTEM_H
#define GAME_COMBATSYSTEM_H

#include <ecs/systems/EntitySystem.h>

class CombatSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

    void update(double deltaTime, EntityEngine *engine) override;

};


#endif
