
description("Simple rope using verlet physics")
defaultArgs({
    draw = false,
    color = colors.wood,
    endPointEntity = nil,
    length = 16,
    moveByWind = 0
})

function create(rope, args)

    components = {
        VerletRope = {
            endPointEntity = args.endPointEntity,
            length = args.length,
            moveByWind = args.moveByWind
        },
        AABB = {
            halfSize = {4, 4}
        }
    }

    if args.draw then

        components.DrawPolyline = {
            colors = { args.color }
        }

    end

    setComponents(rope, components)
end