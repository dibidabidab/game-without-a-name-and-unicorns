
#ifndef GAME_RAINBOWENTITY_H
#define GAME_RAINBOWENTITY_H

#include "EntityTemplate.h"
#include "../components/physics/Physics.h"
#include "../components/physics/PolyPlatform.h"
#include "../components/Polyline.h"

class RainbowEntity : public EntityTemplate
{

  public:
    entt::entity create() override
    {
        entt::entity e = room->entities.create();

        room->entities.assign<PolyPlatform>(e);
        room->entities.assign<PolyPlatformWaves>(e);
        room->entities.assign<AABB>(e, vec2(20), vec2(128));
        Polyline &line = room->entities.assign<Polyline>(e);

        vec2 vel(8, 10);
        vec2 p(-75, -100);
        for (int i = 0; i < 20; i++)
        {
            line.points.push_back(p);
            p += vel;
            vel.y -= .8;
        }

        auto &draw = room->entities.assign<DrawPolyline>(e);
        draw.repeatColors = {5u, 5u, 4u, 4u, 3u, 3u};
        draw.repeatY = 5;
        draw.addAABBOffset = true;

        return e;
    }

};


#endif
