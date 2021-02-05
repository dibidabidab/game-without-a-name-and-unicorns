
persistenceMode(TEMPLATE | ARGS | FINAL_POS)

defaultArgs({
    width = 160,
    rightPolePositionX = 200,
    rightPolePositionY = 100
})

function create(poleLeft, args)

    local poleRight = createChild(poleLeft, "poleRight")

    component.AABB.getFor(poleLeft).halfSize = ivec2(2, 6)

    setComponents(poleLeft, {
        VerletRope {
            endPointEntity = poleRight,
            length = args.width * 1.2,
            nrOfPoints = args.width / 10,
            fixedEndPoint = true,
            moveByWind = .8,
            gravity = vec2(0, -5)
        },
        DrawPolyline {
            colors = {colors.wood}
        }
    })

    setComponents(poleRight, {
        AABB {
            halfSize = ivec2(2, 6),
            center = ivec2(args.rightPolePositionX, args.rightPolePositionY)
        },
    })

    local mainRope = poleLeft -- either makes things less confusing or a lot more. hehe.

    local spacing = 10
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
                gravity = vec2(0, -10),
                friction = .9
            },
            DrawPolyline {
                colors = {colors.wood}
            },
            Flammable {
                range = 16,
                checkInterval = .5,

                addedIgniterComponent = Igniter {
                    igniteChance = .1
                },
                addedFireComponent = Fire {
                    intensity = .4,
                    width = 1
                }
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
            },
            Flammable {
                range = 16,
                checkInterval = .5,

                addedIgniterComponent = Igniter {
                    igniteChance = .1
                },
                addedFireComponent = Fire {
                    intensity = .4,
                    width = 1
                }
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
        AABB {
            center = (component.AABB.getFor(poleLeft).center + component.AABB.getFor(poleRight).center) / ivec2(2)
        },
        DrawPolyline {
            colors = {colors.wood_dark},
            addAABBOffset = true
        }
    })

    if component.Persistent.has(poleLeft) then

        currentEngine.onEvent("BeforeSave", function()

            if not valid(poleLeft) or not valid(poleRight) then
                return
            end

            local pos = component.AABB.tryGetFor(poleRight).center

            local persistent = component.Persistent.tryGetFor(poleLeft)

            if pos ~= nil and persistent ~= nil then

                persistent.data.rightPolePositionX = pos.x
                persistent.data.rightPolePositionY = pos.y
                print(pos)
            end
        end)
    end
end
