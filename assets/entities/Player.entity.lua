
createChild("leftLeg")
createChild("rightLeg")

components = {
    Physics = {
        ignorePolyPlatforms = false,
        createWind = 3,
        ignoreFluids = false
    },
    AABB = {
        halfSize = {3, 13},
        center = {64, 64} -- todo add support for: center = 64 ???
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

transRoomed = getComponent(entity, "TransRoomed")
if transRoomed ~= nil then

    components.AABB.center = transRoomed.positionInNewRoom
    components.Health = transRoomed.archivedComponents.Health
    components.PaletteSetter = transRoomed.archivedComponents.PaletteSetter
    components.Physics.velocity = transRoomed.archivedComponents.Physics.velocity

end

-- HEAD:
applyTemplate(
        createChild("head"),
        "Head",
        {
            body = entity,
            sprite = "sprites/player_head"
        }
)



-- LEGS:
legLength = 15

applyTemplate(leftLeg, "Leg", {
    length = legLength,
    oppositeLeg = rightLeg,
    body = entity,
    hipAnchor = {-3, 0},
    idleXPos = -2,
    spriteSliceName = "leg_left"
})
applyTemplate(rightLeg, "Leg", {
    length = legLength,
    oppositeLeg = leftLeg,
    body = entity,
    hipAnchor = {3, 0},
    idleXPos = 2,
    spriteSliceName = "leg_right"
})



-- ARMS:
armLength = 15

createChild("leftArm")
createChild("rightArm")

applyTemplate(leftArm, "Arm", {
    length = armLength,
    body = entity,
    shoulderAnchor = {-3, 4},
    spriteSliceName = "arm_left"
})
applyTemplate(rightArm, "Arm", {
    length = armLength,
    body = entity,
    shoulderAnchor = {3, 4},
    spriteSliceName = "arm_right"
})



-- BOW WEAPON:
applyTemplate(
        createChild("bow"),
        "Bow",
        {
            archer = entity,
            leftArm = leftArm,
            rightArm = rightArm,
            rotatePivot = {0, 7}
        }
)
