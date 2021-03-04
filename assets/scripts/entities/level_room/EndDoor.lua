
persistenceMode(TEMPLATE | ARGS | SPAWN_POS)

function create(door)

    setName(door, "EndDoor")

    component.AABB.getFor(door).halfSize = ivec2(27, 8)
    setComponents(door, {
        AsepriteView {
            sprite = "sprites/end_door",
            loop = false,
            positionOffset = ivec2(0, 16),
            zIndex = -3
        },
        Sprinkler {
            maxRandomOffset = ivec2(27, 5),
            dropColor = colors.water1,
            maxDropSize = .5,
            amount = 3,
            minLifetime = .4,
            maxLifetime = .85,
            minDropSize = .2,
            maxDropSize = 1.1
        },
        PolyPlatform(),
        Polyline {
            points = {
                vec2(-32, -7),
                vec2(0, 8),
                vec2(32, -7)
            }
        }
    })

    local animate = nil

    animate = function()

        local persistent = component.Persistent.tryGetFor(door)
        if persistent == nil then
            return
        end
        local goal = ivec2(persistent.spawnPosition.x, persistent.spawnPosition.y)
        goal.x = goal.x + math.random(-8, 8)
        goal.y = goal.y + math.random(-8, 8)

        component.AABB.animate(door, "center", goal, .9 + math.random(), "pow2", animate)
    end

    setTimeout(door, .1, animate)

    local detector = createChild(door, "detector")
    setComponents(detector, {
        AABB(),
        SpriteAnchor {
            spriteEntity = door,
            spriteSliceName = "door_center"
        }
    })

    onEntityEvent(detector, "PlayerDetected", function (player)

        setTimeout(detector, 0., function()  -- todo: emitting events from c++ sucks.

            local p = component.Physics.getFor(player)
            p.gravity = 0
            p.velocity = vec2(0)
            component.AABB.animate(player, "center", component.AABB.getFor(detector).center, .3, "pow2Out")
            removeAnimation(door, "center")

            if hudScreen ~= nil and hudScreen.levelFinished ~= nil then
                hudScreen.levelFinished()
            end
        end)
    end)
end
