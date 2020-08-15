
#ifndef GAME_POLYLINE_H
#define GAME_POLYLINE_H

#include "../../../macro_magic/component.h"

/**
 * Basically a list of AABB-entities that act as points for a bezier curve.
 * Entity mush have the DrawPolyline component in order to actually draw the curve to the screen!
 */
COMPONENT(
    BezierCurve,
    HASH(0),
    FIELD(std::vector<entt::entity>, points),
    FIELD_DEF_VAL(int, resolution, 8)
)
END_COMPONENT(BezierCurve)

COMPONENT(
    Polyline,
    HASH(0),
    FIELD(std::list<vec2>, points)
)

    static float heightAtX(int x, const vec2 &p0, const vec2 &p1)
    {
        vec2 delta = p1 - p0;
        if (delta.x == 0)
            return max(p0.y, p1.y);
        return p0.y + delta.y * ((static_cast<float>(x) - p0.x) / delta.x);
    }

    float heightAtX(int x, const vec2 &offset)
    {
        static int i;
        return heightAtX(x, offset, i);
    }

    float heightAtX(int x, const vec2 &offset, int &pointIndex)
    {
        auto it = points.begin();

        for (pointIndex = 0; pointIndex < points.size() - 1; pointIndex++)
        {
            const vec2 p0 = *it + offset;
            const vec2 p1 = *(++it) + offset;

            if (p0.x <= x && p1.x >= x)
                return heightAtX(x, p0, p1);
        }
        return -1;
    }

END_COMPONENT(Polyline)

#endif //GAME_POLYLINE_H
