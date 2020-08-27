
#ifndef GAME_TRANSROOMERSYSTEM_H
#define GAME_TRANSROOMERSYSTEM_H

#include "EntitySystem.h"
#include "../../Level.h"
#include <utils/math_utils.h>

class TransRoomerSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, Room *room) override;

    Room *findNextRoom(Room &current, const ivec2 &travelDir, const ivec2 &position, ivec2 &positionInNextRoom);

};


#endif
