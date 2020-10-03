
#ifndef GAME_TRANSROOMERSYSTEM_H
#define GAME_TRANSROOMERSYSTEM_H

#include <ecs/systems/EntitySystem.h>
#include <utils/math_utils.h>
#include "../../tiled_room/TiledRoom.h"

class TransRoomerSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, EntityEngine *room) override;

    TiledRoom *findNextRoom(TiledRoom &current, const ivec2 &travelDir, const ivec2 &position, ivec2 &positionInNextRoom);

    void archiveComponents(json &j, entt::entity, Room *, const std::vector<std::string> &componentNames);

};


#endif
