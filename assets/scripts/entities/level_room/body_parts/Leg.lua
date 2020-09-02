
defaultArgs({
    length = 15,
    oppositeLeg = nil,
    body = nil,
    hipAnchor = {0, 0},
    idleXPos = 0,
    spriteSliceName = "leg",
    color = 5
})

function create(leg, args)

    -- HIP:
    hip = createChild(leg, "hip")
    applyTemplate(hip, "SpriteAnchor", {
        spriteEntity = args.body,
        sliceName = args.spriteSliceName,
        ignoreSpriteFlipping = true
    })

    -- KNEE:
    knee = createChild(leg, "knee")
    setComponents(knee, {
        AABB = {},
        LimbJoint = {
            hipJointEntity = hip,
            footEntity = leg
        }
    })

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
            points = {hip, knee, leg}
        },
        DrawPolyline = {
            colors = {args.color}
        }
    }
    setComponents(leg, components)
end
