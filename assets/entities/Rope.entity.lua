
arg("draw", false)
arg("color", 1)
arg("endPointEntity", nil)
arg("length", 16)
arg("moveByWind", 0)

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
