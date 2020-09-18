
#ifndef GAME_RAINBOWSYSTEM_H
#define GAME_RAINBOWSYSTEM_H

#include "EntitySystem.h"

class RainbowSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

    EntityEngine *room = NULL;

  protected:
    void update(double deltaTime, EntityEngine *room) override
    {
        this->room = room;

        limitRainbowArrowAngle();

        updateRainbowSpawners(deltaTime);

        updateDisappearingRainbows(deltaTime);
    }

    void limitRainbowArrowAngle();

    void updateRainbowSpawners(float deltaTime);

    void updateDisappearingRainbows(float deltaTime);

};


#endif
