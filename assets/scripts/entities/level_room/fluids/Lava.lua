
description("a pool of lava")

defaultArgs({
    width = 200,
    depth = 100
})

WAVE_MARGIN = 20
PIXELS_PER_LINE = 8

function create(lava, args)

    nrOfPoints = math.ceil(args.width / PIXELS_PER_LINE) + 1

    points = {}

    for i = 0, nrOfPoints - 1 do
        table.insert(points, vec2(
                args.width * -.5 + i * PIXELS_PER_LINE,
                args.depth * .5 - WAVE_MARGIN
        ))
    end

    component.AABB.getFor(lava).halfSize = ivec2(args.width * .5, (args.depth + WAVE_MARGIN) * .5)

    setComponents(lava, {
        Polyline {
            points = points
        },
        DrawPolyline {
            addAABBOffset = true,
            colors = {colors.fire1},
            zIndexBegin = -512, zIndexEnd = -512
        },
        Fluid {
            friction = 8,
            reduceGravity = 1000,

            enterSound = "sounds/jump_in_water",
            leaveSound = "sounds/jump_out_water",

            color = colors.fire0,
            reflective = false,
            splatterAmount = 2.,
            splatterDropSize = 2.5,
            splatterVelocity = 1.,

            bubbleSprite = "sprites/lava_bubble",
            bubblesAmount = 4.
        },
        PolylineWaves {
            moveByWind = .5,
            impactMultiplier = 2.,
            spread = 10,
            dampening = 5,
            stiffness = 520,
        },
        SoundSpeaker {
            sound = "sounds/lava_pool",
            looping = true
        },
        PositionedAudio()
    })

    setUpdateFunction(lava, 70. / args.width, function()

        if math.random() < .3 then
            return
        end

        local aabb = component.AABB.getFor(lava)

        local y = aabb.center.y + aabb.halfSize.y - WAVE_MARGIN
        local x = aabb.center.x + math.random(-aabb.halfSize.x, aabb.halfSize.x)

        local particle = createEntity()
        local lifeTime = math.random() * 1. + .4
        setComponents(particle, {
            AABB {
                center = ivec2(x, y)
            },
            Fire {
                width = 1,
                intensity = .1 + math.random() * .3
            },
            Physics {
                velocity = vec2(-400 + math.random() * 800, math.random() * 500),
                ignoreTileTerrain = true,
                airFriction = 4,
                gravity = 700
            },
            DespawnAfter {
                time = lifeTime
            }
        })
        component.Fire.animate(particle, "intensity", 0., lifeTime)


        local waves = component.PolylineWaves.getFor(lava)
        if #waves.springs > 1 then
            waves.springs[math.random(1, #waves.springs)].velocity = math.random(-200, 200)
        end
    end)
end
