
arg("width", 200)
arg("depth", 100)

WAVE_MARGIN = 32
PIXELS_PER_LINE = 8

nrOfPoints = math.ceil(args.width / PIXELS_PER_LINE) + 1

points = {}

for i = 0, nrOfPoints - 1 do
    table.insert(points, {
        args.width * -.5 + i * PIXELS_PER_LINE,
        args.depth * .5 - WAVE_MARGIN
    })
end

components = {
    AABB = {
        halfSize = {args.width * .5, (args.depth + WAVE_MARGIN) * .5}
    },
    Polyline = {
        points = points
    },
    DrawPolyline = {
        addAABBOffset = true,
        colors = {19},
        zIndexBegin = -512, zIndexEnd = -512
    },
    Fluid = {
        friction = 2.83, -- this value gives a funny walk animation underwater
        reduceGravity = 900,

        enterSound = "sounds/jump_in_water",
        leaveSound = "sounds/jump_out_water",

        color = 18,
        splatterAmount = 4.,
        splatterDropSize = 1.1,
        splatterVelocity = .8
    },
    PolylineWaves = {
        stiffness = 150,
        dampening = 1.6,
        spread = 5,
        moveByWind = 1.0
    }
}
