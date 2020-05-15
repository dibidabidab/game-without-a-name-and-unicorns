
#ifndef GAME_KNEEJOINT_H
#define GAME_KNEEJOINT_H

#include "../../../macro_magic/component.h"

COMPONENT(
    KneeJoint,
    HASH(0),
    FIELD_DEF_VAL(entt::entity, hipJointEntity, entt::null),
    FIELD_DEF_VAL(entt::entity, footEntity, entt::null)
)
END_COMPONENT(KneeJoint)

#endif //GAME_KNEEJOINT_H
