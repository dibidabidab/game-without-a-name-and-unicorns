
#ifndef GAME_DRAWPOLYLINE_H
#define GAME_DRAWPOLYLINE_H

#include "../../../macro_magic/component.h"

/**
 * Used to tell the renderer that a polyline should be drawn.
 * Must be used in combination with BezierCurve, VerletRope or Polyline.
 *
 * The depth value of the drawn line will be interpolated between zIndexBegin and zIndexEnd
 */
COMPONENT(
    DrawPolyline,
    HASH(0),
    FIELD(std::vector<uint8>, colors),
    FIELD_DEF_VAL(float, zIndexBegin, 0),
    FIELD_DEF_VAL(float, zIndexEnd, 0),

    FIELD_DEF_VAL(uint8, repeatX, 0),
    FIELD_DEF_VAL(uint8, repeatY, 0),
    FIELD(std::vector<uint8>, repeatColors),

    FIELD_DEF_VAL(bool, addAABBOffset, false)
)
END_COMPONENT(DrawPolyline)

#endif //GAME_DRAWPOLYLINE_H
