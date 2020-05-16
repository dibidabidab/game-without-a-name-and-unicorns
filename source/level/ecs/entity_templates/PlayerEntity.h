
#ifndef GAME_PLAYERENTITY_H
#define GAME_PLAYERENTITY_H


#include "EntityTemplate.h"
#include "../components/Physics.h"
#include "../components/PlatformerMovement.h"
#include "../components/Networked.h"
#include "../components/PlayerControlled.h"
#include "../components/Light.h"
#include "../components/Leg.h"
#include "../components/SpriteBobbing.h"
#include "../components/AsepriteView.h"
#include "../components/SpriteAnchor.h"
#include "../components/KneeJoint.h"
#include "../components/BezierCurve.h"
#include "../components/DrawPolyline.h"

class PlayerEntity : public EntityTemplate
{
  public:
    entt::entity create() override
    {
        entt::entity e = room->entities.create();

        room->entities.assign<Physics>(e);
        room->entities.assign<AABB>(e, ivec2(3, 13), ivec2(32, 52));
        room->entities.assign<StaticCollider>(e);
        room->entities.assign<PlatformerMovement>(e);

        float legLength = 16;

        std::vector<entt::entity> legEntities{room->entities.create(), room->entities.create()};

        entt::entity legAnchors[2] = {room->entities.create(), room->entities.create()};
        entt::entity knees[2] = {room->entities.create(), room->entities.create()};

        for (int i = 0; i < 2; i++)
        {
            auto opposite = legEntities[i == 0 ? 1 : 0];

            room->entities.assign<Leg>(legEntities[i], legLength, e, ivec2(i == 0 ? -3 : 3, 0), i == 0 ? -2 : 2, opposite, 17.f);
            room->entities.assign<AABB>(legEntities[i], ivec2(1), ivec2(32));


            room->entities.assign<AABB>(legAnchors[i], ivec2(1), ivec2(0));
            room->entities.assign<SpriteAnchor>(legAnchors[i], e, i == 0 ? "leg_left" : "leg_right");

            room->entities.assign<AABB>(knees[i], ivec2(1), ivec2(0));
            room->entities.assign<KneeJoint>(knees[i], legAnchors[i], legEntities[i]);

            // bezier curve:
            room->entities.assign<BezierCurve>(legEntities[i], std::vector<entt::entity>{

                legEntities[i], knees[i], legAnchors[i]

            });
            room->entities.assign<DrawPolyline>(legEntities[i], std::vector<uint8>{5});
        }

        room->entities.assign<SpriteBobbing>(e, legEntities);
        room->entities.assign<AsepriteView>(e, asset<aseprite::Sprite>("sprites/player_body"));

        return e;
    }

    void makeNetworkedServerSide(Networked &networked) override
    {
        networked.toSend.components<PlayerControlled, PlatformerMovement>();

        networked.toSend.group()
                .component<Physics>()
                .interpolatedComponent<AABB>()
                .endGroup();

        networked.toReceive.group()
                .component<Physics>()
                .interpolatedComponent<AABB>()
                .endGroup();
    }

    void makeNetworkedClientSide(Networked &networked) override
    {
        networked.toReceive.components<PlayerControlled, PlatformerMovement>();

        networked.sendIfLocalPlayerReceiveOtherwise.group()
                .component<Physics>()
                .interpolatedComponent<AABB>()
                .endGroup();
    }
};


#endif
