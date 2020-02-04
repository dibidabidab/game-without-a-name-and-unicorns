
#ifndef GAME_PHYSICS_H
#define GAME_PHYSICS_H

#include <utils/math_utils.h>
#include <graphics/3d/debug_line_renderer.h>
#include "../systems/physics/TerrainCollisionDetector.h"
#include "../../Level.h"

/**
 * 2d pixel based Axis Aligned Bounding Box
 */
struct AABB
{
    ivec2 halfSize, center;

    inline ivec2 topRight() const    { return center + halfSize; }
    inline ivec2 bottomRight() const { return center + ivec2(halfSize.x, -halfSize.y); }
    inline ivec2 bottomLeft() const  { return center - halfSize; }
    inline ivec2 topLeft() const     { return center + ivec2(-halfSize.x, halfSize.y); }
};

class PhysicsSystem;

struct Physics
{

    AABB body;
    float gravity = 9.8 * Level::PIXELS_PER_BLOCK;
    vec2 velocity;

    bool ignorePlatforms = false;

    TerrainCollisions touches;

    // used by PhysicsSystem:
    vec2 velocityAccumulator;

    void draw(DebugLineRenderer &lineRenderer, const vec3 &color)
    {
        lineRenderer.line(vec3(body.bottomLeft(), 0), vec3(body.topLeft(), 0), touches.leftWall  ? mu::Y : color);
        lineRenderer.line(vec3(body.topRight(), 0), vec3(body.bottomRight(), 0), touches.rightWall ? mu::Y : color);
        lineRenderer.line(vec3(body.bottomLeft(), 0), vec3(body.bottomRight(), 0), touches.floor     ? mu::Y : color);
        lineRenderer.line(vec3(body.topLeft(), 0), vec3(body.topRight(), 0), touches.ceiling   ? mu::Y : color);
        if (touches.slopeUp)
            lineRenderer.axes(vec3(body.bottomRight(), 0), 2, mu::Y);
        if (touches.slopeDown)
            lineRenderer.axes(vec3(body.bottomLeft(), 0), 2, mu::Y);
        if (touches.slopedCeilingDown)
            lineRenderer.axes(vec3(body.topRight(), 0), 2, mu::Y);
        if (touches.slopedCeilingUp)
            lineRenderer.axes(vec3(body.topLeft(), 0), 2, mu::Y);
    }

};

#endif
