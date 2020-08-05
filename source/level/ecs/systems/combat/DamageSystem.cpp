
#include "DamageSystem.h"
#include "../../../room/Room.h"
#include "../../components/combat/Health.h"
#include "../../components/physics/Physics.h"
#include "../../components/PlayerControlled.h"
#include "../../components/combat/Bow.h"
#include "../../components/graphics/PaletteSetter.h"
#include "../../components/graphics/BloodDrop.h"

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

            Physics *p = room->entities.try_get<Physics>(e);
            AABB *aabb = room->entities.try_get<AABB>(e);

            if (damageType.knockback != 0 && health.knockBackResistance != 1)
            {
                if (p && aabb)
                {
                    vec2 direction = vec2(aabb->center) - attack.sourcePosition;
                    direction = normalize(direction);

                    float force = attack.knockBackMultiplier * damageType.knockback;
                    force *= 1.f - health.knockBackResistance;

                    p->velocity += direction * force;
                }
            }
            if (aabb && health.bloodColor != 0u) for (int i = 0; i < mu::randomInt(10, 30); i++)
            {
                auto dropE = room->entities.create();

                float dropSize = mu::random(.8, 2.5);
                auto &drop = room->entities.assign<BloodDrop>(dropE, dropSize, health.bloodColor);
                if (dropSize > 1.3)
                    drop.splitAtTime = mu::random(.05, .5);

                room->entities.assign<AABB>(dropE, ivec2(1), aabb->center);
                auto &dropPhysics = room->entities.assign<Physics>(dropE);
                dropPhysics.wallFriction = dropPhysics.floorFriction = mu::random(10, 60);
                dropPhysics.velocity = vec2(mu::random(-250, 250), mu::random(-250, 250));
                if (p)
                    dropPhysics.velocity += p->velocity * (mu::random() > .9 ? -.5f : 1.f);
            }

            if (health.currHealth <= attack.points)
            {
                // this attack is the death cause of this entity
                health.currHealth = 0;

                if (!health.givePlayerArrowOnKill.empty() && room->entities.try_get<PlayerControlled>(attack.dealtBy) != NULL)
                {
                    Bow *bow = room->tryGetChildComponentByName<Bow>(attack.dealtBy, "bow");
                    if (bow)
                    {
                        bow->arrowTemplate = health.givePlayerArrowOnKill;

                        PaletteSetter *p = room->entities.try_get<PaletteSetter>(attack.dealtBy);
                        if (p) try
                        {
                            p->paletteName = damageTypesJson.get()["arrowPalettes"][bow->arrowTemplate];
                        }
                        catch (std::exception &)
                        {
                            std::cerr << "No palette name found for " << bow->arrowTemplate << std::endl;
                        }
                    }
                }
                continue;
            }
            health.currHealth -= attack.points;
            health.immunityTimer += damageType.immunity;
        }

        health.attacks.clear();

        if (health.currHealth == 0)
        {
            for (auto &componentName : health.componentsToRemoveOnDeath)
            {
                const ComponentUtils *utils = ComponentUtils::getFor(componentName);
                if (!utils)
                {
                    std::cerr << "componentsToRemoveOnDeath for entity#" << int(e) << " contains '" << componentName << "' which is not a component type!" << std::endl;
                    continue;
                }
                utils->removeComponent(e, room->entities);
            }
            if (health.componentsToAddOnDeath.is_object())
            {
                for (auto &[componentName, component] : health.componentsToAddOnDeath.items())
                {
                    const ComponentUtils *utils = ComponentUtils::getFor(componentName);
                    if (!utils)
                    {
                        std::cerr << "componentsToAddOnDeath for entity#" << int(e) << " contains '" << componentName << "' which is not a component type!" << std::endl;
                        continue;
                    }
                    try
                    {
                        utils->setJsonComponentWithKeys(component, e, room->entities);
                    }
                    catch (std::exception &exc)
                    {
                        std::cerr << "Error while adding " << componentName << " to dead entity#" << int(e) << ":\n" << exc.what() << std::endl;
                    }
                }
            }

            room->entities.remove<Health>(e);
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
        damageTypes = damageTypesJson.get()["damageTypes"].get<decltype(damageTypes)>();
    } catch (std::exception &e)
    {
        throw gu_err("Content of " + damageTypesJson.getLoadedAsset().fullPath + " is invalid!");
    }
}
