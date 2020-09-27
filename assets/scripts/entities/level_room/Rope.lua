
description("Simple rope using verlet physics")
defaultArgs({
    draw = true,
    color = colors.wood,
    endPointEntity = nil,
    length = 16,
    moveByWind = 0,
    updatePrecision = 1
})

function create(rope, args)

    setComponent(rope, VerletRope {
        endPointEntity = args.endPointEntity,
        length = args.length,
        moveByWind = args.moveByWind,
        updatePrecision = args.updatePrecision
    })
    component.AABB.getFor(rope).halfSize = ivec2(4)

    if args.draw then
        component.DrawPolyline.getFor(rope).colors = { args.color }
    end
end