
#include "DamageSystem.h"
#include "../../../room/Room.h"
#include "../../components/combat/Health.h"
#include "../../components/physics/Physics.h"

void DamageSystem::update(double deltaTime, Room *room)
{
    if (damageTypesJson.hasReloaded())
        loadDamageTypes();

    room->entities.view<Health>().each([&](auto e, Health &health) {

        health.immunityTimer = max<float>(0, health.immunityTimer - deltaTime);

        if (health.immunityTimer == 0.) for (auto &attack : health.attacks)
        {
            if (!health.doesTakeDamageType(attack.type))
            {
                // this entity does NOT take damage of this type..

                continue;
            }

            auto t = damageTypes.find(attack.type);
            if (t == damageTypes.end())
            {
                std::cerr << "Trying to attack entity#" << int(e) << " with non-existing damageType:\n" << json(attack).dump(2) << std::endl;
                continue;
            }
            DamageType &damageType = t->second;

            if (damageType.knockback != 0 && health.knockBackResistance != 1)
            {
                Physics *p = room->entities.try_get<Physics>(e);
                AABB *aabb = room->entities.try_get<AABB>(e);
                if (p && aabb)
                {
                    vec2 direction = vec2(aabb->center) - attack.sourcePosition;
                    direction = normalize(direction);

                    float force = attack.knockBackMultiplier * damageType.knockback;
                    force *= 1.f - health.knockBackResistance;

                    p->velocity += direction * force;
                }
            }

            if (health.currHealth <= attack.points)
            {
                // this attack is the death cause of this entity
                health.currHealth = 0;
                continue;
            }
            health.currHealth -= attack.points;
            health.immunityTimer += damageType.immunity;
        }

        health.attacks.clear();

        if (health.currHealth == 0)
        {
            // todo: animation and stuff
            room->entities.destroy(e);
        }
    });
}

DamageSystem::DamageSystem() : EntitySystem("damage/health"), damageTypesJson("damage_types")
{
    loadDamageTypes();
}

void DamageSystem::loadDamageTypes()
{
    try
    {
        damageTypes = damageTypesJson.get().get<decltype(damageTypes)>();
    } catch (std::exception &e)
    {
        throw gu_err("Content of " + damageTypesJson.getLoadedAsset().fullPath + " is invalid!");
    }
}
