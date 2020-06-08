
arg("length", 15)
arg("body", nil)
arg("shoulderAnchor", { 0, 0 })
arg("spriteSliceName", "arm")
arg("color", 5)

-- SHOULDER:
applyTemplate(createChild("shoulder"), "SpriteAnchor", {
    spriteEntity = args.body,
    sliceName = args.spriteSliceName,
    ignoreSpriteFlipping = true
})

-- ELBOW:
createChild("elbow")
childComponents.elbow = {
    AABB = {},
    LimbJoint = {
        hipJointEntity = shoulder,
        footEntity = entity
    }
}

-- HAND/ARM:
components = {
    Arm = {
        length = args.length,
        body = args.body,
        anchor = args.shoulderAnchor,
    },
    AABB = {},
    BezierCurve = {
        points = {shoulder, elbow, entity}
    },
    DrawPolyline = {
        colors = {args.color}
    }
}
