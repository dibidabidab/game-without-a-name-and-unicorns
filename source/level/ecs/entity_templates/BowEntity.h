
#ifndef GAME_BOWENTITY_H
#define GAME_BOWENTITY_H

#include "EntityTemplate.h"
#include "../components/physics/Physics.h"
#include "../components/graphics/AsepriteView.h"
#include "../components/combat/Bow.h"

class BowEntity : public EntityTemplate
{
  public:
    entt::entity create() override
    {
        auto e = room->entities.create();

        Bow &bow = room->entities.get_or_assign<Bow>(e);
        room->entities.assign<AABB>(e, ivec2(3));
        room->entities.assign<AsepriteView>(e, asset<aseprite::Sprite>("sprites/bow"));

        bow.handBowAnchor = room->entities.create();
        room->entities.assign<AABB>(bow.handBowAnchor, ivec2(1));
        room->entities.assign<SpriteAnchor>(bow.handBowAnchor, e, "hand_bow_anchor");

        bow.handStringAnchor = room->entities.create();
        room->entities.assign<AABB>(bow.handStringAnchor, ivec2(1));
        room->entities.assign<SpriteAnchor>(bow.handStringAnchor, e, "hand_string_anchor");

        return e;
    }
};


#endif
