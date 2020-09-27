function create(player)

    leftLeg = createChild(player, "leftLeg")
    rightLeg = createChild(player, "rightLeg")

    setComponents(player, {
        Physics {
            ignorePolyPlatforms = false,
            createWind = 3,
            ignoreFluids = false
        },
        AABB {
            halfSize = ivec2(3, 13),
            center = ivec2(64)
        },
        StaticCollider(),
        PlatformerMovement(),
        Flip(),
        PointLight {
            radius = 60
        },
        Health {
            takesDamageFrom = {"hit"},
            maxHealth = saveGame.maxPlayerHealth or 20,
            currHealth = saveGame.currentPlayerHealth or 20
        },
        Aiming(),
        AsepriteView {
            sprite = "sprites/player_body",
            zIndex = -1 -- make sure head and bow show in front of body
        },
        SpriteBobbing {
            feet = {leftLeg, rightLeg}
        },
        PaletteSetter {
            priority = 1,
            paletteName = "default"
        },
        TransRoomable {
            templateName = TEMPLATE_NAME,
            archiveComponents = {"Health", "PaletteSetter", "Physics"}
        }
    })

    arrowTemplate = "Arrow"

    local transRoomed = component.TransRoomed.tryGetFor(player)

    if transRoomed ~= nil then

        component.AABB.getFor(player).center = transRoomed.positionInNewRoom

        setComponentFromJson(player, "Health", transRoomed.archivedComponents.Health)
        setComponentFromJson(player, "PaletteSetter", transRoomed.archivedComponents.PaletteSetter)
        setComponentFromJson(player, "Physics", transRoomed.archivedComponents.Physics)

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
        hipAnchor = ivec2(-3, 0),
        idleXPos = -2,
        spriteSliceName = "leg_left"
    })
    applyTemplate(rightLeg, "Leg", {
        length = legLength,
        oppositeLeg = leftLeg,
        body = player,
        hipAnchor = ivec2(3, 0),
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
        shoulderAnchor = ivec2(-3, 4),
        spriteSliceName = "arm_left"
    })
    applyTemplate(rightArm, "Arm", {
        length = armLength,
        body = player,
        shoulderAnchor = ivec2(3, 4),
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
            rotatePivot = ivec2(0, 7)
        }
    )
    component.TransRoomable.getFor(player).archiveChildComponents = { bow = {"Bow"} } -- archive the Bow component of the bow child


    local t = 0

    setUpdateFunction(player, 1., function(deltaTime)
        t = t + deltaTime
    end)

    setOnDestroyCallback(player, function()
        print("Player entity #", player, "was alive for", t, "seconds. RIP")

        local health = component.Health.tryGetFor(player)
        if health ~= nil then
            saveGame.currentPlayerHealth = health.currHealth
            saveGame.maxPlayerHealth = health.maxHealth
        end
    end)
end
