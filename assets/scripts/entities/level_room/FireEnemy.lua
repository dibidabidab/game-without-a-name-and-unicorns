
persistenceMode(TEMPLATE | ARGS | SPAWN_POS | REVIVE)

defaultArgs({
    maxFlyDistance = 150,
    triggerDistance = 160
})

FIRE_INTENSITY = .5

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
        },
        PointLight {
            radius = 16,
            radiusFlickeringFrequency = 3.1, -- todo: 3.1 gives slow flickering, 3.2 gives extremely fast flickering
            radiusFlickeringIntensity = 10
        },
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
            sprite = "sprites/fire_enemy",
            frame = 7
        },
        PointLight {
            radius = 32,
            radiusFlickeringFrequency = 3.1, -- todo: 3.1 gives slow flickering, 3.2 gives extremely fast flickering
            radiusFlickeringIntensity = 10
        },
        Fire {
            width = 1,
            intensity = FIRE_INTENSITY
        },
        VerletRope {
            length = 28,
            nrOfPoints = 10,
            endPointEntity = fireTail,
            friction = .99
        },
        DrawPolyline {
            colors = {10}
        },
        PlayerDetector {
            distance = args.triggerDistance
        }
    })

    local playersInRange = 0
    local attackMode = false

    setUpdateFunction(enemy, .8, function()
        if attackMode then
            return
        end

        local velGoal = vec2(math.random(-50, 50),
                            math.random(-50, 50))

        local persistent = component.Persistent.tryGetFor(enemy)
        if persistent ~= nil then

            local aabb = component.AABB.getFor(enemy)

            local spawnPos = ivec2(persistent.spawnPosition.x, persistent.spawnPosition.y)

            local diff = vec2(spawnPos - aabb.center)

            local dist = diff:length()

            if dist > args.maxFlyDistance then
                component.AABB.animate(enemy, "center", spawnPos, .8, "pow3")
                return
            end

            local steerError = (dist / args.maxFlyDistance) ^ 3
            --print(steerError, dist)

            velGoal = velGoal * vec2(1. - steerError) + diff * vec2(steerError)
        end

        component.Physics.animate(enemy, "velocity", velGoal, .8, "pow2")

    end)


    onEntityEvent(enemy, "PlayerDetected", function (player)

        --local playerAABB = component.AABB.getFor(player)
        --local enemyAABB = component.AABB.getFor(enemy)

        playersInRange = playersInRange + 1
        attackMode = playersInRange > 0

        if playersInRange == 1 then

            local sprite = component.AsepriteView.getFor(enemy)
            sprite.loop = false
            playAsepriteTag(sprite, "angry", true)

            local fire = component.Fire.getFor(enemy)
            fire.intensity = 10
            fire.width = 10
            component.Fire.animate(enemy, "intensity", FIRE_INTENSITY, .3, "linear")
            component.Fire.animate(enemy, "width", 1, .3, "linear")

            local sound = component.SoundSpeaker.getFor(enemy)
            sound.sound = "sounds/fire/lit"
            sound.volume  = .2
            sound.pitch = .7

            local dirI = 0
            local rotateFunc = nil
            rotateFunc = function()

                if not attackMode then
                    return
                end
                local newVelocity = ({vec2(-100, 0), vec2(0, -100), vec2(100, 0), vec2(0, 100)})[dirI + 1]

                component.Physics.animate(enemy, "velocity", newVelocity, .25, "linear", rotateFunc)

                dirI = (dirI + 1) % 4
            end
            rotateFunc()

            local particleI = 0

            setUpdateFunction(fireTail, .08, function(dt)

                particleI = 1 + particleI
                if particleI < 8 then
                    return
                end

                local enemyAABB = component.AABB.getFor(enemy)
                local tailAABB = component.AABB.getFor(fireTail)
                local fireParticle = createChild(fireTail)

                local vel = vec2(tailAABB.center - enemyAABB.center) * vec2(8)
                vel = vec2(-vel.y, vel.x)

                setComponents(fireParticle, {
                    Physics {
                        ignoreFluids = false,
                        --ignoreTileTerrain = true,
                        ghost = true,
                        ignorePlatforms = true,
                        airFriction = .5,
                        gravity = 0,
                        velocity = vel
                    },
                    AABB {
                        center = tailAABB.center,
                        halfSize = ivec2(1)
                    },
                    Fire {
                        intensity = .5
                    },
                    Igniter {
                        igniteChance = 1.,
                        range = 16
                    },
                    DespawnAfter {
                        time = .25
                    }
                })
            end)
        end
    end)

    onEntityEvent(enemy, "PlayerGone", function (player)

        playersInRange = playersInRange - 1
        attackMode = playersInRange > 0

        if playersInRange == 0 then
            component.Fire.remove(fireTail)
            setUpdateFunction(fireTail, 99, nil)
        end
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
