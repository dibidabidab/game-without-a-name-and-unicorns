
defaultArgs({
    length = 15,
    body = nil,
    shoulderAnchor = ivec2(),
    spriteSliceName = "arm",
    color = 5
})

function create(arm, args)

    -- SHOULDER:
    shoulder = createChild(arm, "shoulder")
    applyTemplate(shoulder, "SpriteAnchor", {
        spriteEntity = args.body,
        sliceName = args.spriteSliceName,
        ignoreSpriteFlipping = true
    })

    -- ELBOW:
    elbow = createChild(arm, "elbow")
    setComponents(elbow, {
        AABB(),
        LimbJoint {
            hipJointEntity = shoulder,
            footEntity = arm
        }
    })

    -- HAND/ARM:
    components = {
        Arm {
            length = args.length,
            body = args.body,
            anchor = args.shoulderAnchor,
        },
        AABB(),
        BezierCurve {
            points = {shoulder, elbow, arm}
        },
        DrawPolyline {
            colors = {args.color}
        }
    }
    setComponents(arm, components)
end
