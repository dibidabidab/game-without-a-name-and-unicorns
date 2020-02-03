
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
};

class PhysicsSystem;

struct Physics
{

    AABB body;
    float gravity = 9.8 * Level::PIXELS_PER_BLOCK;
    vec2 velocity;

    TerrainCollisions touches;

    // used by PhysicsSystem:
    vec2 velocityAccumulator;

    void draw(DebugLineRenderer &lineRenderer, const vec3 &color)
    {
        lineRenderer.line(vec3(body.center - body.halfSize, 0), vec3(body.center - ivec2(body.halfSize.x, -body.halfSize.y), 0), touches.leftWall  ? mu::Y : color);
        lineRenderer.line(vec3(body.center + body.halfSize, 0), vec3(body.center + ivec2(body.halfSize.x, -body.halfSize.y), 0), touches.rightWall ? mu::Y : color);
        lineRenderer.line(vec3(body.center - body.halfSize, 0), vec3(body.center - ivec2(-body.halfSize.x, body.halfSize.y), 0), touches.floor     ? mu::Y : color);
        lineRenderer.line(vec3(body.center + body.halfSize, 0), vec3(body.center + ivec2(-body.halfSize.x, body.halfSize.y), 0), touches.ceiling   ? mu::Y : color);
    }

};

#endif
