
#ifndef GAME_LIMBJOINT_H
#define GAME_LIMBJOINT_H

#include "../../../../macro_magic/component.h"

/**
 * The position of an entity with AABB and LimbJoint will be based on
 * the position of the hip and foot (or shoulder and hand), using simple Inverse Kinematics
 */
COMPONENT(
    LimbJoint,
    HASH(0),
    FIELD_DEF_VAL(entt::entity, hipJointEntity, entt::null),
    FIELD_DEF_VAL(entt::entity, footEntity, entt::null),
    FIELD_DEF_VAL(bool, invert, false)
)
END_COMPONENT(LimbJoint)

#endif //GAME_LIMBJOINT_H
