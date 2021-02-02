function create(player)

    local leftLeg = createChild(player, "leftLeg")
    local rightLeg = createChild(player, "rightLeg")

    setComponents(player, {
        Physics {
            ignorePolyPlatforms = false,
            createWind = 1.5,
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
            radius = 48
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

    local arrowTemplate = "DefaultArrow"

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

    local leftArm = createChild(player, "leftArm")
    local rightArm = createChild(player, "rightArm")

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
    local bow = createChild(player, "bow")
    applyTemplate(
        bow, "Bow",
        {
            arrowTemplate = arrowTemplate,
            archer = player,
            leftArm = leftArm,
            rightArm = rightArm,
            rotatePivot = ivec2(0, 7)
        }
    )
    component.TransRoomable.getFor(player).archiveChildComponents = { bow = {"Bow"} } -- archive the Bow component of the bow child

    onEntityEvent(player, "Attacked", function(attack)
        print("oof")
    end)

    local hud_arrowTypeBar = nil

    if hudScreen ~= nil then
        hudScreen.applyTemplate(hudScreen.createEntity(), "HealthBar", {
            entityRoom = currentEngine,
            entity = player
        })

        hud_arrowTypeBar = hudScreen.createEntity()

        hudScreen.applyTemplate(hud_arrowTypeBar, "ArrowTypeBar", {
            entityRoom = currentEngine,
            archer = player,
            bow = bow
        })
    end


    setOnDestroyCallback(player, function()

        local health = component.Health.tryGetFor(player)
        if health ~= nil then
            saveGame.currentPlayerHealth = health.currHealth
            saveGame.maxPlayerHealth = health.maxHealth
        end

        if hudScreen.valid(hud_arrowTypeBar) then -- hud_arrowTypeBar might be nil
            hudScreen.destroyEntity(hud_arrowTypeBar)
        end
    end)
end
