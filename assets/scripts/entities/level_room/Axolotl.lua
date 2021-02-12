
persistenceMode(TEMPLATE | ARGS | SPAWN_POS | REVIVE)

function create(axo)

    local leftLeg = createChild(axo, "leftLeg")
    local rightLeg = createChild(axo, "rightLeg")

    setComponents(axo, {

        AsepriteView {
            sprite = "sprites/axolotl"
        },
        Physics {
            gravity = 820,
            ignoreFluids = false
        },
        PlatformerMovement {
            walkVelocity = 40,
            jumpVelocity = 100
        },
        PlatformerMovementInput(),

        SpriteBobbing {
            feet = {leftLeg, rightLeg},
            maxYPos = 4,
            minYPos = -2
        }

    })
    component.AABB.getFor(axo).halfSize = ivec2(10, 6)


    local tail = createChild(axo, "tail")
    setComponents(tail, {
        AABB {
            halfSize = ivec2(1)
        },
        VerletRope {
            length = 10,
            nrOfPoints = 4,
            gravity = vec2(2, 0),
            friction = .9
        },
        DrawPolyline {
            colors = { colors.brick },
            lineWidth = 3
        },
        SpriteAnchor {
            spriteEntity = axo,
            spriteSliceName = "tail"
        }
    })
    local tailEnd = createChild(tail, "tailEnd")
    setComponents(tailEnd, {
        AttachToRope {
            ropeEntity = tail,
            x = 1
        },
        AABB {
            halfSize = ivec2(1)
        },
        VerletRope {
            length = 9,
            nrOfPoints = 4,
            gravity = vec2(2, 1),
            friction = .95
        },
        DrawPolyline {
            colors = { colors.brick },
            lineWidth = 2
        }
    })



    -- LEGS:
    legLength = 5

    applyTemplate(leftLeg, "Leg", {
        length = legLength,
        oppositeLeg = rightLeg,
        body = axo,
        hipAnchor = ivec2(1, -3),
        idleXPos = -2,
        spriteSliceName = "leg0",
        stepSize = 5,
        color = colors.brick
    })
    applyTemplate(rightLeg, "Leg", {
        length = legLength,
        oppositeLeg = leftLeg,
        body = axo,
        hipAnchor = ivec2(7, -3),
        idleXPos = 2,
        spriteSliceName = "leg1",
        stepSize = 5,
        color = colors.brick
    })
end
