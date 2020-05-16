
#ifndef GAME_BEZIERCURVE_H
#define GAME_BEZIERCURVE_H

#include "../../../macro_magic/component.h"

COMPONENT(
    BezierCurve,
    HASH(0),
    FIELD(std::vector<entt::entity>, points),
    FIELD_DEF_VAL(int, resolution, 8)
)
END_COMPONENT(BezierCurve)

#endif //GAME_BEZIERCURVE_H
