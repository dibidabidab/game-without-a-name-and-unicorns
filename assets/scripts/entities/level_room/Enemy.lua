
persistenceMode(TEMPLATE | ARGS | SPAWN_POS | REVIVE)

function create(enemy)
    component.AABB.getFor(enemy).halfSize = ivec2(5, 8)
    setComponents(enemy, {
        Physics {
            ignorePolyPlatforms = false,
            ignoreFluids = false
        },
        StaticCollider(),
        Health {
            takesDamageFrom = {"hit"},
            componentsToAddOnDeath = {
                SliceSpriteIntoPieces = {
                    steps = 6
                }
            },
            currHealth = 4,
            maxHealth = 4,
            givePlayerArrowOnKill = "RainbowArrow"
        },
        AsepriteView {
            sprite = "sprites/enemy"
        },
        PointLight {
            radius = 60
        }
    })

    onEntityEvent(enemy, "Damage", function(damage, removeListener)

        local health = component.Health.getFor(enemy)

        print("AUWWWWWW", damage.points, health.currHealth, "/", health.maxHealth)

        if health.currHealth == 1 then
            removeListener()
        end

    end)

end
