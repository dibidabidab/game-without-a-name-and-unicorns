
persistenceMode(TEMPLATE | ARGS | SPAWN_POS | REVIVE)

function create(enemy)
    component.AABB.getFor(enemy).halfSize = ivec2(9, 8)
    setComponents(enemy, {
        Physics {
            --ignoreFluids = false,
            gravity = 0
        },
        StaticCollider(),
        Health {
            takesDamageFrom = {"water"},
            currHealth = 1,
            maxHealth = 1,
            givePlayerArrowOnKill = "FireArrow"
        },
        AsepriteView {
            sprite = "sprites/fire_enemy"
        },
        PointLight {
            radius = 60,
            radiusFlickeringFrequency = 3.1, -- todo: 3.1 gives slow flickering, 3.2 gives extremely fast flickering
            radiusFlickeringIntensity = 10
        },
        Fire {
            width = 1,
            intensity = .5
        }
    })

    onEntityEvent(enemy, "Attacked", function(attack)

        local health = component.Health.getFor(enemy)

        print("AUWWWWWW", attack.points, health.currHealth, "/", health.maxHealth)
    end)
    onEntityEvent(enemy, "Died", function (attack)
        component.SliceSpriteIntoPieces.getFor(enemy).steps = 6

        print("enemy was killed with an attack of", attack.points, "points")
    end)
end
