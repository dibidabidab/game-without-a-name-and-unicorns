
// todo: move this AABB bullshit to a better place

#ifndef GAME_COMPONENT_METHODS_H
#define GAME_COMPONENT_METHODS_H

#include <graphics/3d/debug_line_renderer.h>
#include <utils/math_utils.h>
#include "../../generated/Physics.hpp"

inline void draw(const AABB &aabb, DebugLineRenderer &lineRenderer, const vec3 &color)
{
    lineRenderer.line(aabb.bottomLeft(), aabb.topLeft(), color);
    lineRenderer.line(aabb.topRight(), aabb.bottomRight(), color);
    lineRenderer.line(aabb.bottomLeft(), aabb.bottomRight(), color);
    lineRenderer.line(aabb.topLeft(), aabb.topRight(), color);
}

inline bool overlap(const AABB &a, const AABB &b)
{
    return
        b.center.x - b.halfSize.x < a.center.x + a.halfSize.x
        &&
        b.center.x + b.halfSize.x > a.center.x - a.halfSize.x
        &&
        b.center.y - b.halfSize.y < a.center.y + a.halfSize.y
        &&
        b.center.y + b.halfSize.y > a.center.y - a.halfSize.y;
}

inline vec2 randomPointInAABB(const AABB &aabb)
{
    return vec2(
            aabb.center.x + mu::random(-aabb.halfSize.x, aabb.halfSize.x),
            aabb.center.y + mu::random(-aabb.halfSize.y, aabb.halfSize.y)
    );
}

namespace
{

    enum OutCode: uint8
    {
        INSIDE = 0,
        LEFT = 1,
        RIGHT = 2,
        BOTTOM = 4,
        TOP = 8
    };

    template <class vec>
    inline uint8 computeOutCode(const AABB &aabb, const vec &p)
    {
        uint8 code = INSIDE;
        if (p.x < aabb.center.x - aabb.halfSize.x)
            code |= LEFT;
        else if (p.x > aabb.center.x + aabb.halfSize.x)
            code |= RIGHT;
        if (p.y < aabb.center.y - aabb.halfSize.y)
            code |= BOTTOM;
        else if (p.y > aabb.center.y + aabb.halfSize.y)
            code |= TOP;
        return code;
    }
}

/**
* based on: https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
*/
template <class vec>
inline bool lineIntersectsOrInside(const AABB &aabb, const vec &p0, const vec &p1)
{
    auto code0 = computeOutCode(aabb, p0), code1 = computeOutCode(aabb, p1);
    if (!(code0 | code1))
    {
        // both points inside AABB; trivially accept
        return true;
    }
    else if (code0 & code1)
    {
        // both points share an outside zone (LEFT, RIGHT, TOP, or BOTTOM), so both must be outside AABB
        return false;
    }
    else return
        mu::lineSegmentsIntersect(p0, p1, aabb.bottomLeft(), aabb.topLeft())
        || mu::lineSegmentsIntersect(p0, p1, aabb.bottomRight(), aabb.topRight())
        || mu::lineSegmentsIntersect(p0, p1, aabb.bottomLeft(), aabb.bottomRight())
        || mu::lineSegmentsIntersect(p0, p1, aabb.topLeft(), aabb.topRight());
}

template <>
struct ComponentInterpolator<AABB>
{
    float speed = 5;

    AABB diff(AABB& aabb, const AABB &other)
    {
        prevCenter = aabb.center;
        aabb.copyFieldsFrom(other);

        if (length(vec2(other.center - prevCenter)) < 32) // dont interpolate if distance is too big
            aabb.center = prevCenter;

        AABB diff;
        diff.center = other.center - aabb.center;
        return diff;
    }

    vec2 addAccumulator;
    ivec2 prevCenter;

    void add(AABB& aabb, const AABB &diff, float x)
    {
        addAccumulator += vec2(diff.center) * vec2(x);

        ivec2 selfTravel = aabb.center - prevCenter;
        while (selfTravel.x >= 1 && addAccumulator.x > 0)
            addAccumulator.x -= 1;
        while (selfTravel.x <= -1 && addAccumulator.x < 0)
            addAccumulator.x += 1;
        while (selfTravel.y >= 1 && addAccumulator.y > 0)
            addAccumulator.y -= 1;
        while (selfTravel.y <= -1 && addAccumulator.y < 0)
            addAccumulator.y += 1;

        aabb.center += addAccumulator;
        vec2 one(1);
        addAccumulator = modf(addAccumulator, one);


        prevCenter = aabb.center;
    }
};

inline void draw(const Physics &physics, const AABB &body, DebugLineRenderer &lineRenderer, const vec3 &color)
{
    lineRenderer.line(body.bottomLeft(), body.topLeft(), physics.touches.leftWall    ? mu::Y : color);
    lineRenderer.line(body.topRight(), body.bottomRight(), physics.touches.rightWall ? mu::Y : color);
    lineRenderer.line(body.bottomLeft(), body.bottomRight(), physics.touches.floor   ? mu::Y : color);
    lineRenderer.line(body.topLeft(), body.topRight(), physics.touches.ceiling       ? mu::Y : color);
    if (physics.touches.slopeUp)
        lineRenderer.axes(body.bottomRight(), 2, mu::Y);
    if (physics.touches.slopeDown)
        lineRenderer.axes(body.bottomLeft(), 2, mu::Y);
    if (physics.touches.slopedCeilingDown)
        lineRenderer.axes(body.topRight(), 2, mu::Y);
    if (physics.touches.slopedCeilingUp)
        lineRenderer.axes(body.topLeft(), 2, mu::Y);
    if (physics.touches.polyPlatform)
        lineRenderer.circle(body.bottomCenter(), 2, 8, mu::Y);
    if (physics.touches.pixelsAbovePolyPlatform != 0)
        lineRenderer.line(body.bottomCenter(), body.bottomCenter() - ivec2(0, physics.touches.pixelsAbovePolyPlatform), vec3(1, 1, 0));
    if (physics.touches.fluid)
        lineRenderer.circle(body.center, min(body.halfSize.x, body.halfSize.y), 8, color);
}

#endif //GAME_COMPONENT_METHODS_H
