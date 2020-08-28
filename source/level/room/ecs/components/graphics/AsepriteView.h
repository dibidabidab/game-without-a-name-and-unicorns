
#ifndef GAME_ASEPRITEVIEW_H
#define GAME_ASEPRITEVIEW_H

#include "../../../../../macro_magic/component.h"
#include "../physics/Physics.h"
#include <utils/aseprite/Aseprite.h>
#include <asset_manager/asset.h>

SERIALIZABLE(
    SpriteClipping,
    FIELD(u8vec2, lowerLeft),
    FIELD(u8vec2, lowerRight),
    FIELD(u8vec2, topLeft),
    FIELD(u8vec2, topRight)
)
END_SERIALIZABLE(SpriteClipping)

/**
 * Used to draw an (animated) sprite.
 * Entity must have the AABB component to determine where on the screen the sprite is drawn
 */
COMPONENT(
    AsepriteView,

    HASH(sprite.getLoadedAsset().shortPath, playingTag, paused, zIndex, aabbAlign.x, aabbAlign.y, originAlign.x, originAlign.y),

    FIELD(asset<aseprite::Sprite>, sprite),
    FIELD_DEF_VAL(int, frame, 0),
    FIELD_DEF_VAL(int, playingTag, -1),
    FIELD_DEF_VAL(bool, paused, false),
    FIELD_DEF_VAL(bool, loop, true),
    FIELD_DEF_VAL(bool, flipHorizontal, false),
    FIELD_DEF_VAL(bool, flipVertical, false),
    FIELD_DEF_VAL(bool, rotate90Deg, false),
    FIELD_DEF_VAL(float, zIndex, 0),

    FIELD_DEF_VAL(vec2, aabbAlign, vec2(.5)), // by default the sprite's origin is positioned at the center of the AABB of the entity
    FIELD_DEF_VAL(vec2, originAlign, vec2(.5)), // by default the sprite's origin is the center of the sprite.
    FIELD_DEF_VAL(ivec2, positionOffset, ivec2(0)),

    FIELD(SpriteClipping, clip)
)

    float frameTimer = 0;
    bool pong = false;

    /**
     * play a tag animation.
     * returns the duration of the animation in seconds.
     */
    float playTag(const char *tagName, bool unpause=false)
    {
        int i =  0;
        for (auto &tag : sprite->tags)
        {
            if (tag.name == tagName)
            {
                frame = tag.from;
                frameTimer = 0;
                pong = false;
                playingTag = i;
                paused = unpause ? false : paused;
                return tag.duration;
            }
            i++;
        }
        return 0;
    }

    ivec2 getDrawPosition(const AABB &aabb) const
    {
        ivec2 position = aabb.bottomLeft();
        position += positionOffset;
        position += originAlign * vec2(-sprite->width, -sprite->height);
        position += aabbAlign * vec2(aabb.halfSize * 2);
        return position;
    }

END_COMPONENT(AsepriteView)


COMPONENT(
    SliceSpriteIntoPieces,
    HASH(0),
    FIELD_DEF_VAL(int, steps, 1)
)
END_COMPONENT(SliceSpriteIntoPieces)

#endif //GAME_ASEPRITEVIEW_H
