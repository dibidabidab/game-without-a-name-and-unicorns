
#ifndef GAME_SPAWNING_H
#define GAME_SPAWNING_H

#include "../../../macro_magic/component.h"

COMPONENT(
    TemplateSpawner,
    HASH(templateName),
    FIELD(std::string, templateName),
    FIELD(json, arguments)
)
END_COMPONENT(TemplateSpawner)

COMPONENT(
    SpawnedBy,
    HASH(0),
    FIELD(entt::entity, spawner)
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
