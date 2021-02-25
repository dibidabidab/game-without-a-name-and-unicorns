
local bloodSounds = include("scripts/entities/level_room/_blood_sounds")

function create(player)

    setName(player, "Player")

    local leftLeg = createChild(player, "leftLeg")
    local rightLeg = createChild(player, "rightLeg")

    setComponents(player, {
        Physics {
            ignorePolyPlatforms = false,
            createWind = 1.5,
            ignoreFluids = false,
            autoStepHeight = 10
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
            takesDamageFrom = {"hit", "fire", "water"},
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
        },
        PositionedAudio()
    })
    local spawnPoint = getByName("Spawnpoint")
    if spawnPoint ~= nil then
        local spawnPointAABB = component.AABB.tryGetFor(spawnPoint)
        if spawnPointAABB ~= nil then
            component.AABB.getFor(player).center = spawnPointAABB.center
        end
    end

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
    local head = createChild(player, "head")
    applyTemplate(
        head,
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

    onEntityEvent(player, "ArrowTemplateFromKilledEntity", function()

        setUpdateTimeMultiplier(.5, .1)
    end)

    onEntityEvent(player, "LaunchedArrowReflected", function()

        setUpdateTimeMultiplier(.4, .07)
    end)

    bloodSounds.addTo(player)

    onEntityEvent(player, "Attacked", function(attack)
        setUpdateTimeMultiplier(.5, .05)

        component.CameraShaking.getFor(player).intensity = 2
        setTimeout(player, .1, component.CameraShaking.remove)

        component.SoundSpeaker.remove(player)
        local ss = component.SoundSpeaker.getFor(player)
        ss.sound = "sounds/ouch"
        ss.pitch = math.random() * .4 + .6
        ss.volume = .3
    end)
    onEntityEvent(player, "Died", function (attack)
        component.TransRoomable.remove(player)

        -- BOW:
        local fakeBow = createEntity()
        setComponents(fakeBow, {
            component.AABB.getFor(bow),
            component.AsepriteView.getFor(bow),
            Physics {
                velocity = vec2(math.random(-1000, 1000), 700)
            },
            DespawnAfter {
                time = 10.
            }
        })
        destroyEntity(bow)
        component.Arm.getFor(leftArm).grab = nil
        component.Arm.getFor(rightArm).grab = nil

        -- HEAD:
        component.Child.remove(head)
        component.SpriteAnchor.remove(head)
        component.Head.remove(head)
        component.Physics.getFor(head).velocity = vec2(math.random(-500, 500), 900)
        component.DespawnAfter.getFor(head).time = 60.
        component.PlayerControlled.getFor(head) -- a hack to keep the room alive.

        -- BODY:
        component.SliceSpriteIntoPieces.getFor(player).steps = 4

        if saveGame.timesDied ~= nil then
            saveGame.timesDied = saveGame.timesDied + 1
        else
            saveGame.timesDied = 1
        end

        setTimeout(player, .2, destroyEntity)

        setTimeout(head, 1., function()
            if hudScreen ~= nil then
                hudScreen.applyTemplate(hudScreen.createEntity(), "YouDiedMessage")
            end
        end)
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
