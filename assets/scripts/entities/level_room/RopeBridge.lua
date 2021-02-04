
defaultArgs({
    width = 160
})


function create(poleLeft, args)

    local poleRight = createChild(poleLeft, "poleRight")

    local poleComponents = {
        AABB {
            halfSize = ivec2(2),
            center = ivec2(32)
        },
        Inspecting()
    }

    setComponents(poleLeft, poleComponents)
    setComponents(poleRight, poleComponents)

    setComponents(poleLeft, {
        VerletRope {
            endPointEntity = poleRight,
            length = args.width * 1.2,
            nrOfPoints = args.width / 10,
            fixedEndPoint = true,
            moveByWind = .4,
            gravity = vec2(0, -5)
        },
        DrawPolyline {
            colors = {colors.wood}
        }
    })

    local mainRope = poleLeft -- either makes things less confusing or a lot more. hehe.

    local spacing = 16
    local nrOfPlanks = math.floor(args.width / spacing)
    local planks = {}

    for i = 1, nrOfPlanks do

        local attachPoint = createChild(mainRope, "attachpoint_"..i)
        local plank = createChild(attachPoint, "plank_"..i)
        planks[i] = plank

        setComponents(attachPoint, {
            AABB(),
            AttachToRope {
                ropeEntity = mainRope,
                x = (i - .5) / nrOfPlanks
            },
            VerletRope {
                endPointEntity = plank,
                length = 24,
                nrOfPoints = 5,
                --fixedEndPoint = true,
                gravity = vec2(0, -4),
                friction = .98
            },
            DrawPolyline {
                colors = {colors.wood}
            }
        })

        setComponents(plank, {
            AABB {
                halfSize = ivec2(4, 1),
                center = component.AABB.getFor(poleLeft).center + ivec2(args.width / 2, 0)
            },
            --Polyline {
            --    points = {vec2(-8, 0), vec2(8, 0)}
            --},
            --PolyPlatform {
            --    allowFallThrough = false
            --}
            AsepriteView {
                sprite = "sprites/rope_bridge",
                frame = 1
            }
        })
    end

    local polyPlatform = createChild(poleLeft, "polyPlatform")
    setComponents(polyPlatform, {
        PolylineFromEntities {
            pointEntities = planks
        },
        PolyPlatform {
            allowFallThrough = false
        },
        AABB(),
        Inspecting(),
        DrawPolyline {
            colors = {colors.wood},
            addAABBOffset = true
        }
    })
end
