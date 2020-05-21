
#ifndef GAME_PHYSICS_H
#define GAME_PHYSICS_H

#include <utils/math_utils.h>
#include <graphics/3d/debug_line_renderer.h>
#include "../../systems/physics/TerrainCollisionDetector.h"
#include "../../../Level.h"
#include "../../../room/TileMap.h"
#include "../../../../macro_magic/component.h"
#include "../../systems/networking/NetworkedData.h"

/**
 * 2d pixel based Axis Aligned Bounding Box
 */
COMPONENT(
    AABB,

    HASH(
        halfSize.x, halfSize.y,
        center.x / centerHashStep, center.y / centerHashStep
    ),

    FIELD(ivec2, halfSize),
    FIELD_DEF_VAL(ivec2, center, ivec2(0))
)
    int centerHashStep = 10;

    inline ivec2 topRight() const       { return center + halfSize; }
    inline ivec2 bottomRight() const    { return center + ivec2(halfSize.x, -halfSize.y); }
    inline ivec2 bottomLeft() const     { return center - halfSize; }
    inline ivec2 topLeft() const        { return center + ivec2(-halfSize.x, halfSize.y); }
    inline ivec2 topCenter() const      { return center + ivec2(0, halfSize.y); }
    inline ivec2 bottomCenter() const   { return center + ivec2(0, -halfSize.y); }
    inline ivec2 rightCenter() const    { return center + ivec2(halfSize.x, 0); }
    inline ivec2 leftCenter() const     { return center + ivec2(-halfSize.x, 0); }

    void draw(DebugLineRenderer &lineRenderer, const vec3 &color) const
    {
        lineRenderer.line(bottomLeft(), topLeft(), color);
        lineRenderer.line(topRight(), bottomRight(), color);
        lineRenderer.line(bottomLeft(), bottomRight(), color);
        lineRenderer.line(topLeft(), topRight(), color);
    }

    template <class vec>
    bool contains(const vec &p) const
    {
        return p.x >= center.x - halfSize.x && p.x <= center.x + halfSize.x && p.y >= center.y - halfSize.y && p.y <= center.y + halfSize.y;
    }

    bool overlaps(const AABB &other) const
    {
        return
            other.center.x - other.halfSize.x < center.x + halfSize.x
            &&
            other.center.x + other.halfSize.x > center.x - halfSize.x
            &&
            other.center.y - other.halfSize.y < center.y + halfSize.y
            &&
            other.center.y + other.halfSize.y > center.y - halfSize.y;
    }

END_COMPONENT(AABB)

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

/**
 * Adds physics to an AABB-entity. (Terrain collisions & gravity)
 */
COMPONENT(
    Physics,

    HASH(
        int(velocity.x) / 50, int(velocity.y) / 50, int(gravity), bool(velocity.x == 0), bool(velocity.y == 0), // todo this is probably broken since the introduction of friction.
        airFriction, floorFriction, wallFriction, ignorePlatforms
    ),

    FIELD_DEF_VAL   (float, gravity, 1200),
    FIELD_DEF_VAL   (float, airFriction, 1.6),
    FIELD_DEF_VAL   (float, floorFriction, 10),
    FIELD_DEF_VAL   (float, wallFriction, 4),
    FIELD_DEF_VAL   (vec2,  velocity, vec2(0)),
    FIELD_DEF_VAL   (bool,  ignorePlatforms, false)
)

    TerrainCollisions touches, prevTouched;
    vec2 prevVelocity;

    float airTime = 0;

    // used by PhysicsSystem:
    vec2 velocityAccumulator;

    void draw(const AABB &body, DebugLineRenderer &lineRenderer, const vec3 &color) const
    {
        lineRenderer.line(body.bottomLeft(), body.topLeft(), touches.leftWall    ? mu::Y : color);
        lineRenderer.line(body.topRight(), body.bottomRight(), touches.rightWall ? mu::Y : color);
        lineRenderer.line(body.bottomLeft(), body.bottomRight(), touches.floor   ? mu::Y : color);
        lineRenderer.line(body.topLeft(), body.topRight(), touches.ceiling       ? mu::Y : color);
        if (touches.slopeUp)
            lineRenderer.axes(body.bottomRight(), 2, mu::Y);
        if (touches.slopeDown)
            lineRenderer.axes(body.bottomLeft(), 2, mu::Y);
        if (touches.slopedCeilingDown)
            lineRenderer.axes(body.topRight(), 2, mu::Y);
        if (touches.slopedCeilingUp)
            lineRenderer.axes(body.topLeft(), 2, mu::Y);
    }

END_COMPONENT(Physics)

/**
 * Entities that are a StaticCollider can push entities that are DynamicCollider.
 */
COMPONENT(
    StaticCollider,
    HASH(0),
    FIELD(char, _)
)
END_COMPONENT(StaticCollider)

COMPONENT(
    DynamicCollider,
    HASH(repositionAfterCollision),

    FIELD_DEF_VAL(bool, repositionAfterCollision, true)
)
END_COMPONENT(DynamicCollider)

COMPONENT(
    DistanceConstraint,
    HASH(maxDistance),
    FIELD_DEF_VAL(float, maxDistance, 32),
    // todo add min distance

    FIELD_DEF_VAL(entt::entity, target, entt::null),
    FIELD_DEF_VAL(vec2, targetOffset, vec2(0))
)
END_COMPONENT(DistanceConstraint)

#endif
