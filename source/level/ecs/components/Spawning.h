
#ifndef GAME_SPAWNING_H
#define GAME_SPAWNING_H

#include "../../../macro_magic/component.h"

COMPONENT(
    TemplateSpawner,
    HASH(templateName),
    FIELD(std::string, templateName),
    FIELD_DEF_VAL(float, minDelay, 1),
    FIELD_DEF_VAL(float, maxDelay, 2),
    FIELD_DEF_VAL(float, minQuantity, 1),
    FIELD_DEF_VAL(float, maxQuantity, 10),
    FIELD_DEF_VAL(json, customData, json::object())
)

    float timer = 0, nextTime = 0;

END_COMPONENT(TemplateSpawner)

COMPONENT(
    SpawnedBy,
    HASH(0),
    FIELD(entt::entity, spawner),
    FIELD(json, customData),
    FIELD(ivec2, spawnerPos)
)
END_COMPONENT(SpawnedBy)

COMPONENT(
    DespawnAfter,
    HASH(time),
    FIELD(float, time),
    FIELD_DEF_VAL(float, timer, 0)
)
END_COMPONENT(DespawnAfter)

#endif //GAME_SPAWNING_H
