
#ifndef GAME_CHILDREN_H
#define GAME_CHILDREN_H

#include "../../../macro_magic/component.h"

typedef std::map<std::string, entt::entity> entity_name_map;

COMPONENT(
    Parent,
    HASH(0),
    FIELD_DEF_VAL(bool, deleteChildrenOnDeletion, true),
    FIELD(final<std::vector<entt::entity>>, children),
    FIELD(final<entity_name_map>, childrenByName)
)
END_COMPONENT(Parent)

COMPONENT(
    Child,
    HASH(0),
    FIELD(final<entt::entity>, parent),
    FIELD(final<std::string>, name)
)
END_COMPONENT(Child)

#endif //GAME_CHILDREN_H
