
description("a pool of water")

defaultArgs({
    width = 200,
    depth = 100
})

WAVE_MARGIN = 20
PIXELS_PER_LINE = 8

function create(water, args)

    nrOfPoints = math.ceil(args.width / PIXELS_PER_LINE) + 1

    points = {}

    for i = 0, nrOfPoints - 1 do
        table.insert(points, vec2(
            args.width * -.5 + i * PIXELS_PER_LINE,
            args.depth * .5 - WAVE_MARGIN
        ))
    end

    setComponents(water, {
        AABB {
            halfSize = ivec2(args.width * .5, (args.depth + WAVE_MARGIN) * .5)
        },
        Polyline {
            points = points
        },
        DrawPolyline {
            addAABBOffset = true,
            colors = {colors.water1},
            zIndexBegin = -512, zIndexEnd = -512
        },
        Fluid {
            friction = 2.83, -- this value gives a funny walk animation underwater
            reduceGravity = 900,

            enterSound = "sounds/jump_in_water",
            leaveSound = "sounds/jump_out_water",

            color = colors.water0,
            splatterAmount = 4.,
            splatterDropSize = 1.1,
            splatterVelocity = .8,

            bubbleSprite = "sprites/bubble",
            bubblesAmount = 1.
        },
        PolylineWaves {
            moveByWind = 1.,
            impactMultiplier = 2.,
            spread = 60
        }
    })
end
