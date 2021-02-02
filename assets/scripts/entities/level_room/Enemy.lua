
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
            currHealth = 2,
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

    onEntityEvent(enemy, "Attacked", function(attack)

        local health = component.Health.getFor(enemy)

        print("AUWWWWWW", attack.points, health.currHealth, "/", health.maxHealth)
    end)
    onEntityEvent(enemy, "Died", function (attack)
        component.SliceSpriteIntoPieces.getFor(enemy).steps = 6

        print("enemy was killed with an attack of", attack.points, "points")
    end)

    --setUpdateFunction(enemy, .03, function(deltaTime)
    --    local arrow = createEntity()
    --    applyTemplate(arrow, "DefaultArrow")
    --
    --    component.Arrow.getFor(arrow).launchedBy = enemy
    --    component.AABB.getFor(arrow).center = component.AABB.getFor(enemy).center
    --    component.Physics.getFor(arrow).velocity = vec2(math.random(-500, 500), math.random(-500, 500))
    --end)
end
