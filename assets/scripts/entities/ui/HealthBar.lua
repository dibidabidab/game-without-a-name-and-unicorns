

defaultArgs({
    entityRoom = nil,
    entity = nil
})


function create(hpBar, args)

    local NR_OF_HEARTS = 7
    local HEART_WIDTH = 8

    setComponents(hpBar, {
        UIElement {
            absolutePositioning = true,
            absoluteHorizontalAlign = 1,
            absoluteVerticalAlign = 2
        },
        UIContainer {
            nineSliceSprite = "sprites/ui/hp_bar_9slice",
            fixedHeight = 27,
            fixedWidth = 32 + NR_OF_HEARTS * HEART_WIDTH,
            centerAlign = true
        }
    })

    local heartsRow = createChild(hpBar)
    setComponents(heartsRow, {
        UIElement(),
        UIContainer {
            fixedHeight = 3,
            fixedWidth = NR_OF_HEARTS * HEART_WIDTH
        }
    })

    local heartEntities = {}

    for i = 1, NR_OF_HEARTS do
        local heart = createChild(heartsRow)
        heartEntities[i] = heart
        setComponents(heart, {
            UIElement(),
            AsepriteView {
                sprite = "sprites/ui/heart"
            }
        })
    end

    local text = createChild(hpBar)
    applyTemplate(text, "Text", {
        waving = false,
        wavingFrequency = 3,
        wavingSpeed = 40,
        wavingAmplitude = 1.5
    })

    local prevHealth = nil
    local prevMaxHealth = nil

    local textFlashTimer = 0
    local heartFrames = nil
    local FRAMES_PER_HEART = 5

    local updateFunction = function(deltaTime)

        if not args.entityRoom.valid(args.entity) then
            destroyEntity(hpBar)
            return
        end

        local currHealth = 0
        local maxHealth = prevMaxHealth

        local healthComp = args.entityRoom.component.Health.tryGetFor(args.entity)
        if healthComp ~= nil then
            currHealth = healthComp.currHealth
            maxHealth = healthComp.maxHealth
        end

        local textView = component.TextView.getFor(text)
        textView.text = "HP: "..tostring(currHealth).."/"..tostring(maxHealth)

        textView.waving = currHealth < maxHealth * .2 and currHealth < 6

        if prevHealth ~= nil and currHealth < prevHealth then
            textFlashTimer = .3
        end

        textView.mapColorTo = math.floor(textFlashTimer * 100) % 2 == 0 and 7 or 10

        textFlashTimer = math.max(0, textFlashTimer - deltaTime)


        -- hearts:
        local heartFramesGoal = math.ceil((currHealth / maxHealth) * FRAMES_PER_HEART * NR_OF_HEARTS)

        if heartFramesGoal ~= heartFrames then

            heartFrames = heartFrames or heartFramesGoal

            if heartFrames < heartFramesGoal then
                heartFrames = heartFrames + 1
            elseif heartFrames > heartFramesGoal then
                heartFrames = heartFrames - 1
            end

            for i = 1, NR_OF_HEARTS do

                local spriteView = component.AsepriteView.getFor(heartEntities[i])
                spriteView.frame = math.max(0, math.min(FRAMES_PER_HEART - 1, heartFrames - (i - 1) * FRAMES_PER_HEART))

            end
        end

        prevHealth = currHealth
        prevMaxHealth = maxHealth
    end

    setUpdateFunction(hpBar, 1/30, updateFunction)
    updateFunction(0)

    -- remove HP bar from screen if the entity's room gets destroyed (TODO: entity itself can emit a event before being destroyed too, right?)
    args.entityRoom.onEvent("BeforeDelete", function()
        if valid(hpBar) then -- todo, move this check to destroyEntity() and similar functions?
            destroyEntity(hpBar)
        end
    end)
end

