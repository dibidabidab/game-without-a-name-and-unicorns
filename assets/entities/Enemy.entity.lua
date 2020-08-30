
persistenceMode(TEMPLATE | ARGS | SPAWN_POS)

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
            currHealth = 2,
            maxHealth = 2,
            givePlayerArrowOnKill = "RainbowArrow"
        },
        AsepriteView = {
            sprite = "sprites/enemy"
        },
        PointLight = {
            radius = 60
        }
    })
end
