
defaultArgs({
    length = 15,
    oppositeLeg = nil,
    body = nil,
    hipAnchor = ivec2(),
    idleXPos = 0,
    spriteSliceName = "leg",
    color = 5,

    stepSize = 13,
    stepArcAngle = 65,
    idleStepSpeed = 65,
    inAirStepSpeed = 40,

    ignoreSpriteFlipping = true
})

function create(leg, args)

    -- HIP:
    hip = createChild(leg, "hip")
    applyTemplate(hip, "SpriteAnchor", {
        spriteEntity = args.body,
        sliceName = args.spriteSliceName,
        ignoreSpriteFlipping = args.ignoreSpriteFlipping
    })

    -- KNEE:
    knee = createChild(leg, "knee")
    setComponents(knee, {
        AABB(),
        LimbJoint {
            hipJointEntity = hip,
            footEntity = leg
        }
    })

    -- FOOT/LEG:
    components = {
        Leg {
            length = args.length,
            body = args.body,
            anchor = args.hipAnchor,
            idleXPos = args.idleXPos,
            oppositeLeg = args.oppositeLeg,

            stepSize = args.stepSize,
            stepArcAngle = args.stepArcAngle,
            idleStepSpeed = args.idleStepSpeed,
            inAirStepSpeed = args.inAirStepSpeed
        },
        AABB(),
        BezierCurve {
            points = {hip, knee, leg}
        },
        DrawPolyline {
            colors = {args.color}
        }
    }
    setComponents(leg, components)
end
