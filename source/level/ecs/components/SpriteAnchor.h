
#ifndef GAME_SPRITEANCHOR_H
#define GAME_SPRITEANCHOR_H

#include "../../../macro_magic/component.h"

COMPONENT(
    SpriteAnchor,
    HASH(0),
    FIELD_DEF_VAL(entt::entity, spriteEntity, entt::null),
    FIELD(std::string, spriteSliceName)
)
END_COMPONENT(SpriteAnchor)

#endif //GAME_SPRITEANCHOR_H
