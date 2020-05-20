//
// Created by sneeuwpop on 20-05-20.
//

#ifndef GAME_KNOCKBACKSYSTEM_H
#define GAME_KNOCKBACKSYSTEM_H

#include "../EntitySystem.h"
#include "../../../Level.h"
#include "../../components/physics/Physics.h"
#include "../../components/combat/KnockBack.h"

/**
 * give an thing a tmp push
 */
class KnockBackSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

protected:
    void update(double deltaTime, Room *room) override {
        room->entities.view<KnockBack, Physics, AABB>().each(
            [&](auto e, KnockBack &knockBack, Physics &physics, AABB &aabb)
        {
            physics.velocity += knockBack.knockBackDirection * knockBack.knockBackForce;
            knockBack.knockBackForce = 0.0f;

//            knockBack.knockBackForce *= 0.8f;
//            if (knockBack.knockBackForce < 0.01f)
//            {
//                knockBack.knockBackForce = 0.0f;
//            }
        });
    }
};

#endif //GAME_KNOCKBACKSYSTEM_H
