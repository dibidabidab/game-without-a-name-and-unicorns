
#ifndef GAME_ENTITYREFERENCE_H
#define GAME_ENTITYREFERENCE_H


#include "../../../entt/src/entt/fwd.hpp"
#include "../room/Room.h"

struct EntityReference
{

    Room *room = NULL;
    entt::entity entity = entt::null;

};

inline void to_json(json &j, const EntityReference &ref)
{
    if (ref.room == NULL || ref.entity == entt::null)
    {
        j = json();
        assert(j.is_null());
        return;
    }
    j = ref.entity;
    // todo make this work in multiplayer
}

inline void from_json(const json &j, EntityReference &ref)
{
    if (j.is_null())
    {
        ref.entity = entt::null;
        return;
    }
    ref.entity = j;
    // todo make this work in multiplayer
}

#endif
