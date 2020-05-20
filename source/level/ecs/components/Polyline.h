
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
END_COMPONENT(Polyline)

#endif //GAME_POLYLINE_H
