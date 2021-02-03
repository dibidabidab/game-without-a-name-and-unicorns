
persistenceMode(TEMPLATE | ARGS | SPAWN_POS | REVIVE)

defaultArgs({
    maxFlyDistance = 150
})

function create(enemy, args)
    component.AABB.getFor(enemy).halfSize = ivec2(9, 8)

    local fireTail = createChild(enemy, "tail")
    setComponents(fireTail, {
        AABB {
            halfSize = ivec2(1)
        },
        --Fire {
        --    width = 1,
        --    intensity = .4
        --},
        AsepriteView {
            sprite = "sprites/fire_enemy_tail"
        }
    })

    setComponents(enemy, {
        Physics {
            --ignoreFluids = false,
            ignorePlatforms = true,
            airFriction = .5,
            gravity = 0,
            createWind = 1
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
        },
        VerletRope {
            length = 28,
            nrOfPoints = 10,
            endPointEntity = fireTail,
            friction = .99
        },
        DrawPolyline {
            colors = {10}
        }
    })

    setUpdateFunction(enemy, .8, function()

        local velGoal = vec2(math.random(-50, 50),
                            math.random(-50, 50))

        local persistent = component.Persistent.tryGetFor(enemy)
        if persistent ~= nil then

            local aabb = component.AABB.getFor(enemy)

            local spawnPos = ivec2(persistent.spawnPosition.x, persistent.spawnPosition.y)

            local diff = vec2(spawnPos - aabb.center)

            local dist = diff:length()

            if dist > args.maxFlyDistance then
                component.AABB.animate(enemy, "center", spawnPos, .5, "pow3")
                return
            end

            local steerError = (dist / args.maxFlyDistance) ^ 3
            --print(steerError, dist)

            velGoal = velGoal * vec2(1. - steerError) + diff * vec2(steerError)
        end

        component.Physics.animate(enemy, "velocity", velGoal, .8, "pow2")

    end)

    onEntityEvent(enemy, "Attacked", function(attack)

        local health = component.Health.getFor(enemy)

        print("AUWWWWWW", attack.points, health.currHealth, "/", health.maxHealth)
    end)
    onEntityEvent(enemy, "Died", function (attack)
        component.SliceSpriteIntoPieces.getFor(enemy).steps = 6

        print("enemy was killed with an attack of", attack.points, "points")
    end)
end
