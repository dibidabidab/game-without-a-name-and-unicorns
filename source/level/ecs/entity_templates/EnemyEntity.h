//
// Created by sneeuwpop on 20-05-20.
//

#ifndef GAME_ENEMYENTITY_H
#define GAME_ENEMYENTITY_H

#include "EntityTemplate.h"
#include "../components/physics/Physics.h"
#include "../components/graphics/Light.h"
#include "RopeEntity.h"
#include "../components/graphics/AsepriteView.h"
#include "../components/graphics/DrawPolyline.h"
#include "../components/combat/Health.h"
#include "../components/combat/KnockBack.h"
#include "../components/generators/TextGenerator.h"

class EnemyEntity : public EntityTemplate
{
public:
    entt::entity create() override
    {
        entt::entity e = room->entities.create();

        room->entities.assign<AABB>(e, ivec2(5, 8));
        room->entities.assign<AsepriteView>(e, asset<aseprite::Sprite>("sprites/enemy"));
        room->entities.assign<KnockBack>(e);
        room->entities.assign<Health>(e);
        room->entities.assign<StaticCollider>(e);
        room->entities.assign<Physics>(e);
        room->entities.assign<TextGenerator>(e, "Hello");

        return e;
    }
};

#endif //GAME_ENEMYENTITY_H
