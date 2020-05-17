
#ifndef GAME_KNEEJOINT_H
#define GAME_KNEEJOINT_H

#include "../../../macro_magic/component.h"

/**
 * The position of an entity with AABB and KneeJoint will be based on
 * the position of the hip and foot. (using simple Inverse Kinematics)
 */
COMPONENT(
    KneeJoint,
    HASH(0),
    FIELD_DEF_VAL(entt::entity, hipJointEntity, entt::null),
    FIELD_DEF_VAL(entt::entity, footEntity, entt::null)
)
END_COMPONENT(KneeJoint)

#endif //GAME_KNEEJOINT_H
