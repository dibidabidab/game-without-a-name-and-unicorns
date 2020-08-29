
function create(player)

    leftLeg = createChild(player, "leftLeg")
    rightLeg = createChild(player, "rightLeg")

    components = {
        Physics = {
            ignorePolyPlatforms = false,
            createWind = 3,
            ignoreFluids = false
        },
        AABB = {
            halfSize = {3, 13},
            center = {64, 64}
        },
        StaticCollider = {},
        PlatformerMovement = {},
        Flip = {},
        PointLight = {
            radius = 60
        },
        Health = {
            takesDamageFrom = {"hit"}
        },
        Aiming = {},
        AsepriteView = {
            sprite = "sprites/player_body",
            zIndex = -1 -- make sure head and bow show in front of body
        },
        SpriteBobbing = {
            feet = {leftLeg, rightLeg}
        },
        PaletteSetter = {
            priority = 1,
            paletteName = "default"
        },
        TransRoomable = {
            templateName = TEMPLATE_NAME,
            archiveComponents = {"Health", "PaletteSetter", "Physics"}
        }
    }

    arrowTemplate = "Arrow"

    transRoomed = getComponent(player, "TransRoomed")
    if transRoomed ~= nil then

        components.AABB.center = transRoomed.positionInNewRoom
        components.Health = transRoomed.archivedComponents.Health
        components.PaletteSetter = transRoomed.archivedComponents.PaletteSetter
        components.Physics.velocity = transRoomed.archivedComponents.Physics.velocity

        arrowTemplate = transRoomed.archivedChildComponents.bow.Bow.arrowTemplate
    end

    -- HEAD:
    applyTemplate(
        createChild(player, "head"),
        "Head",
        {
            body = player,
            sprite = "sprites/player_head"
        }
    )

    -- LEGS:
    legLength = 15

    applyTemplate(leftLeg, "Leg", {
        length = legLength,
        oppositeLeg = rightLeg,
        body = player,
        hipAnchor = {-3, 0},
        idleXPos = -2,
        spriteSliceName = "leg_left"
    })
    applyTemplate(rightLeg, "Leg", {
        length = legLength,
        oppositeLeg = leftLeg,
        body = player,
        hipAnchor = {3, 0},
        idleXPos = 2,
        spriteSliceName = "leg_right"
    })



    -- ARMS:
    armLength = 15

    leftArm = createChild(player, "leftArm")
    rightArm = createChild(player, "rightArm")

    applyTemplate(leftArm, "Arm", {
        length = armLength,
        body = player,
        shoulderAnchor = {-3, 4},
        spriteSliceName = "arm_left"
    })
    applyTemplate(rightArm, "Arm", {
        length = armLength,
        body = player,
        shoulderAnchor = {3, 4},
        spriteSliceName = "arm_right"
    })



    -- BOW WEAPON:
    applyTemplate(
        createChild(player, "bow"),
        "Bow",
        {
            arrowTemplate = arrowTemplate,
            archer = player,
            leftArm = leftArm,
            rightArm = rightArm,
            rotatePivot = {0, 7}
        }
    )
    components.TransRoomable.archiveChildComponents = { bow = {"Bow"} } -- archive the Bow component of the bow child

    setComponents(player, components)
    setUpdateFunction(player, update, 1)
end

function update(player, deltaTime)

    print(player, deltaTime)

end
