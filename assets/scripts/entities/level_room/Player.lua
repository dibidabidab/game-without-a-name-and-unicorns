function create(player)

    --print(component)
    --
    --print(component.StaticCollider.getFor(player).gravity)

    local c = Child:new{
        name = "pepepe"
    }
    --setComponent_new(player, c)
    --print(c.name, "lua")

    setComponents_new(player, {
        Inspecting:new{},
        c
    })

    component.Child.getFor(player).name = "lol"


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
            takesDamageFrom = {"hit"},
            maxHealth = saveGame.maxPlayerHealth or 20,
            currHealth = saveGame.currentPlayerHealth or 20
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

    if component.TransRoomed.has(player) then

        local transRoomed = getComponent(player, "TransRoomed") -- component.TransRoomed.getFor(player) -- todo: tryGetFor()

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

    local t = 0

    setUpdateFunction(player, 1., function(deltaTime)
        t = t + deltaTime
    end)

    setOnDestroyCallback(player, function()
        print("Player entity #", player, "was alive for", t, "seconds. RIP")

        local health = getComponent(player, "Health")
        if health ~= nil then
            saveGame.currentPlayerHealth = health.currHealth
            saveGame.maxPlayerHealth = health.maxHealth
        end
    end)
end
