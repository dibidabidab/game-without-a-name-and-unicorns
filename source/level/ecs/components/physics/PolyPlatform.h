
#ifndef GAME_POLYPLATFORM_H
#define GAME_POLYPLATFORM_H

#include "../../../../macro_magic/component.h"
#include "../Polyline.h"

COMPONENT(
    PolyPlatform,
    HASH(allowFallThrough),
    FIELD_DEF_VAL(bool, allowFallThrough, true)
)

    std::vector<entt::entity> entitiesOnPlatform, entitiesAbovePlatform;

    ivec2 prevAABBPos = ivec2(0);

    static float heightAtX(int x, const vec2 &p0, const vec2 &p1)
    {
        vec2 delta = p1 - p0;
        if (delta.x == 0)
            return max(p0.y, p1.y);
        return p0.y + delta.y * ((static_cast<float>(x) - p0.x) / delta.x);
    }

    static float heightAtX(int x, const Polyline &line, const AABB &platformAABB)
    {
        auto it = line.points.begin();

        for (int i = 0; i < line.points.size() - 1; i++)
        {
            const vec2 p0 = *it + vec2(platformAABB.center);
            const vec2 p1 = *(++it) + vec2(platformAABB.center);

            if (p0.x <= x && p1.x >= x)
                return heightAtX(x, p0, p1);
        }
        return 0;
    }


END_COMPONENT(PolyPlatform)


COMPONENT(
    PolyPlatformWaves,
    HASH(stiffness, dampening, spread),
    FIELD_DEF_VAL(float, stiffness, 240),
    FIELD_DEF_VAL(float, dampening, 4),
    FIELD_DEF_VAL(float, spread, 45)
)

    struct Spring
    {
        float
            yOffset = 0, velocity = 0;
    };

    std::vector<Spring> springs;

END_COMPONENT(PolyPlatformWaves)

#endif //GAME_POLYPLATFORM_H
