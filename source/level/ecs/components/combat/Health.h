//
// Created by sneeuwpop on 20-05-20.
//

#ifndef GAME_HEALTH_H
#define GAME_HEALTH_H

#include "../../../../macro_magic/component.h"
#include <set>
#include <map>

SERIALIZABLE(
    DamageType,
    FIELD_DEF_VAL(float, knockback, 0),
    FIELD_DEF_VAL(float, immunity, 0)
)
END_SERIALIZABLE_FULL_JSON(DamageType)

SERIALIZABLE(
    Damage,
    FIELD(std::string, type),
    FIELD_DEF_VAL(int, points, 0),
    FIELD_DEF_VAL(entt::entity, dealtBy, entt::null),
    FIELD(vec2, sourcePosition),
    FIELD_DEF_VAL(float, knockBackMultiplier, 1)
)
END_SERIALIZABLE_FULL_JSON(Damage)

/**
 * Health of a enemy.
 */
COMPONENT(
    Health,
    HASH(currHealth, maxHealth),
    FIELD_DEF_VAL(uint16, currHealth, 20),
    FIELD_DEF_VAL(uint16, maxHealth, 20),
    FIELD(std::set<std::string>, takesDamageFrom),
    FIELD(std::list<Damage>, attacks),
    FIELD_DEF_VAL(float, immunityTimer, 0),
    FIELD_DEF_VAL(float, knockBackResistance, 0.) // should be between 0 and 1. Values outside that range can create funny results I think
)

    bool doesTakeDamageType(const std::string& damageType) const
    {
        return takesDamageFrom.find(damageType) != takesDamageFrom.end();
    }

END_COMPONENT(Health)

#endif //GAME_HEALTH_H
