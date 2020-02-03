
#ifndef GAME_PHYSICS_H
#define GAME_PHYSICS_H

#include <utils/math_utils.h>
#include <graphics/3d/debug_line_renderer.h>

/**
 * 2d pixel based Axis Aligned Bounding Box
 */
struct AABB
{
    ivec2 halfSize, center;

    void draw(DebugLineRenderer &lineRenderer, const vec3 &color)
    {
        lineRenderer.line(vec3(center - halfSize, 0), vec3(center - ivec2(halfSize.x, -halfSize.y), 0), color);
        lineRenderer.line(vec3(center + halfSize, 0), vec3(center + ivec2(halfSize.x, -halfSize.y), 0), color);
        lineRenderer.line(vec3(center - halfSize, 0), vec3(center - ivec2(-halfSize.x, halfSize.y), 0), color);
        lineRenderer.line(vec3(center + halfSize, 0), vec3(center + ivec2(-halfSize.x, halfSize.y), 0), color);
    }
};

class PhysicsSystem;

struct Physics
{

    AABB body;
    float gravity = 9.8 * Level::PIXELS_PER_BLOCK;
    vec2 velocity;

    vec2 velocityAccumulator;

};

#endif
