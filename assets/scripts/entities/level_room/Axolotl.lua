
local bloodSounds = include("scripts/entities/level_room/_blood_sounds")

persistenceMode(TEMPLATE | ARGS | SPAWN_POS | REVIVE)

function create(axo)

    local leftLeg = createChild(axo, "leftLeg")
    local rightLeg = createChild(axo, "rightLeg")

    setComponents(axo, {

        AsepriteView {
            sprite = "sprites/axolotl",
            loop = false
        },
        Physics {
            gravity = 1050,
            ignoreFluids = false
        },
        PlatformerMovement {
            walkVelocity = 40,
            jumpVelocity = 100,
            swimVelocity = 700
        },
        PlatformerMovementInput(),

        SpriteBobbing {
            feet = {leftLeg, rightLeg},
            maxYPos = 4,
            minYPos = -4
        },

        Health {
            takesDamageFrom = {"hit", "fire"},
            maxHealth = 3,
            currHealth = 3,
            givePlayerArrowOnKill = "WaterArrow"
        }

    })
    component.AABB.getFor(axo).halfSize = ivec2(10, 6)
    playAsepriteTag(component.AsepriteView.getFor(axo), "happy", true)


    local tail = createChild(axo, "tail")
    local TAIL_GRAVITY = vec2(2, 0)
    setComponents(tail, {
        AABB {
            halfSize = ivec2(1)
        },
        VerletRope {
            length = 10,
            nrOfPoints = 4,
            gravity = TAIL_GRAVITY,
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
    local TAIL_END_GRAVITY = vec2(2, 1)
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
            gravity = TAIL_END_GRAVITY,
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
        color = colors.brick,
        ignoreSpriteFlipping = false
    })
    applyTemplate(rightLeg, "Leg", {
        length = legLength,
        oppositeLeg = leftLeg,
        body = axo,
        hipAnchor = ivec2(7, -3),
        idleXPos = 2,
        spriteSliceName = "leg1",
        stepSize = 5,
        color = colors.brick,
        ignoreSpriteFlipping = false
    })

    bloodSounds.addTo(axo, true)

    onEntityEvent(axo, "Attacked", function(attack)

        local sprite = component.AsepriteView.tryGetFor(axo)
        if sprite ~= nil then

            playAsepriteTag(sprite, "cry", true)
        end
    end)
    onEntityEvent(axo, "Died", function (attack)
        local sprite = component.AsepriteView.tryGetFor(axo)
        if sprite ~= nil then

            playAsepriteTag(sprite, "cry", true)
        end

        component.SliceSpriteIntoPieces.getFor(axo).steps = 4

        component.DespawnAfter.getFor(axo).time = .1

        setUpdateFunction(axo, 1., nil)
        component.PlatformerMovementInput.remove(axo)

        print("This species is critically endangered! :'(")
    end)

    setUpdateFunction(axo, 1., function()

        local physics = component.Physics.getFor(axo)

        local xDir = 0

        if math.random() > .5 then
            xDir = xDir - 1
        end
        if math.random() > .5 then
            xDir = xDir + 1
        end

        local input = component.PlatformerMovementInput.getFor(axo)
        input.left = xDir == -1
        input.right = xDir == 1

        if physics.touches.fluid then
            input.jump = ((physics.touches.leftWall or physics.touches.rightWall) and math.random() > .2) or math.random() > .7
        else
            input.jump = false
        end

        if xDir ~= 0 then
            component.Flip.getFor(axo).horizontal = xDir == 1
            component.VerletRope.getFor(tail).gravity = TAIL_GRAVITY * vec2(-xDir, 1)
            component.VerletRope.getFor(tailEnd).gravity = TAIL_END_GRAVITY * vec2(-xDir, 1)

            local l0 = component.Leg.getFor(leftLeg)
            l0.anchor.x = math.abs(l0.anchor.x) * -xDir
            local l1 = component.Leg.getFor(rightLeg)
            l1.anchor.x = math.abs(l1.anchor.x) * -xDir
        end
    end)
end
