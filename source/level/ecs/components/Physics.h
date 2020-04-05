
#ifndef GAME_PHYSICS_H
#define GAME_PHYSICS_H

#include <utils/math_utils.h>
#include <graphics/3d/debug_line_renderer.h>
#include "../systems/physics/TerrainCollisionDetector.h"
#include "../../Level.h"
#include "../../../macro_magic/component.h"

namespace nlohmann {
    template <int len, typename type, qualifier something>
    struct adl_serializer<glm::vec<len, type, something>> {
        static void to_json(json& j, const glm::vec<len, type, something>& v) {
            j = json::array();
            for (int i = 0; i < len; i++)
                j[i] = v[i];
        }

        static void from_json(const json& j, glm::vec<len, type, something>& v) {
            for (int i = 0; i < len; i++)
                v[i] = j[i];
        }
    };
}

/**
 * 2d pixel based Axis Aligned Bounding Box
 */
REFLECTABLE_STRUCT(
    AABB,

    FIELD(ivec2, halfSize),
    FIELD(ivec2, center)
)

    inline ivec2 topRight() const    { return center + halfSize; }
    inline ivec2 bottomRight() const { return center + ivec2(halfSize.x, -halfSize.y); }
    inline ivec2 bottomLeft() const  { return center - halfSize; }
    inline ivec2 topLeft() const     { return center + ivec2(-halfSize.x, halfSize.y); }

END_REFLECTABLE_STRUCT(AABB)


COMPONENT(
    Physics,

    HASH(body.halfSize.x, body.halfSize.y, int(velocity.x) / 10, int(velocity.y) / 10, body.center.x / 10, body.center.y / 10),

    FIELD           (AABB,  body),
    FIELD_DEF_VAL   (float, gravity, 9.8 * Level::PIXELS_PER_BLOCK),
    FIELD           (vec2,  velocity),
    FIELD_DEF_VAL   (bool,  ignorePlatforms, false)
)

    TerrainCollisions touches;

    // used by PhysicsSystem:
    vec2 velocityAccumulator;

    void draw(DebugLineRenderer &lineRenderer, const vec3 &color) const
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


#endif
