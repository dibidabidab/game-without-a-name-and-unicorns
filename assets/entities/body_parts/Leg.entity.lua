
arg("length", 15)
arg("oppositeLeg", nil)
arg("body", nil)
arg("hipAnchor", { 0, 0 })
arg("idleXPos", 0)
arg("spriteSliceName", "leg")
arg("color", 5)

-- HIP:
applyTemplate(createChild("hip"), "SpriteAnchor", {
    spriteEntity = args.body,
    sliceName = args.spriteSliceName,
    ignoreSpriteFlipping = true
})

-- KNEE:
createChild("knee")
childComponents.knee = {
    AABB = {},
    LimbJoint = {
        hipJointEntity = hip,
        footEntity = entity
    }
}

-- FOOT/LEG:
components = {
    Leg = {
        length = args.length,
        body = args.body,
        anchor = args.hipAnchor,
        idleXPos = args.idleXPos,
        oppositeLeg = args.oppositeLeg
    },
    AABB = {},
    BezierCurve = {
        points = {hip, knee, entity}
    },
    DrawPolyline = {
        colors = {args.color}
    }
}
