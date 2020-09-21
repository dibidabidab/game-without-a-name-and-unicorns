
persistenceMode(TEMPLATE | ARGS | SPAWN_POS | REVIVE)

function create(enemy)
    setComponents(enemy, {
        AABB = {
            halfSize = {5, 8}
        },
        Physics = {
            ignorePolyPlatforms = false,
            ignoreFluids = false
        },
        StaticCollider = {},
        Health = {
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
        AsepriteView = {
            sprite = "sprites/enemy"
        },
        PointLight = {
            radius = 60
        }
    })

    onEntityEvent(enemy, "Damage", function(damage, removeListener)

        local health = getComponent(enemy, "Health");

        print("AUWWWWWW", damage.points, health.currHealth, "/", health.maxHealth)

        if health.currHealth == 1 then
            removeListener()
        end

    end)

end
