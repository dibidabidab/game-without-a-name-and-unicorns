
#ifndef GAME_SPRITEANCHOR_H
#define GAME_SPRITEANCHOR_H

#include "../../../../../macro_magic/component.h"

/**
 * Aseprite sprites have 'slices' (https://www.aseprite.org/docs/slices/)
 * Using this component you can attach an entity to a slice's position.
 */
COMPONENT(
    SpriteAnchor,
    HASH(0),
    FIELD_DEF_VAL(entt::entity, spriteEntity, entt::null),
    FIELD(std::string, spriteSliceName),
    FIELD_DEF_VAL(bool, ignoreSpriteFlipping, false)
)
END_COMPONENT(SpriteAnchor)

#endif //GAME_SPRITEANCHOR_H
