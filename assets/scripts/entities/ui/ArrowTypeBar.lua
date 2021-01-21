
str_utils = include("scripts/util/string_utils")

defaultArgs({
    entityRoom = nil,
    archer = nil,
    bow = nil
})

function create(bar, args)

    setComponents(bar, {
        UIElement {
            absolutePositioning = true,
            absoluteHorizontalAlign = 0,
            absoluteVerticalAlign = 0
        },
        UIContainer {
            nineSliceSprite = sprite_asset("sprites/ui/arrow_type_bar_9slice"),
            fixedHeight = 27,
            fixedWidth = 32,
            autoWidth = true
        }
    })

    local bowComponent = args.entityRoom.component.Bow.getFor(args.bow)

    local icon = createChild(bar)
    local iconRenderOffset = ivec2(-25, 2)

    local spriteView = AsepriteView {
        sprite = sprite_asset("sprites/ui/arrow_type_icons"),
        loop = false
    }
    _, spriteView.frame = asepriteTagFrames(spriteView, bowComponent.arrowTemplate)

    setComponents(icon, {
        UIElement {
            absolutePositioning = true,
            renderOffset = iconRenderOffset
        },
        spriteView
    })


    local text = createChild(bar)
    applyTemplate(text, "Text", {
        text = str_utils.camelCaseToSpaces(bowComponent.arrowTemplate),
        wavingFrequency = .1,
        wavingSpeed = -9
    })

    args.entityRoom.onEntityEvent(args.archer, "ArrowTemplateFromKilledEntity", function(killedEntity)

        local killedAABB = args.entityRoom.component.AABB.tryGetFor(killedEntity)
        local bowComponent = args.entityRoom.component.Bow.tryGetFor(args.bow)
        if killedAABB == nil or bowComponent == nil then
            print("hmmmmm")
            return
        end

        local screenKillPos = args.entityRoom.positionToScreenPosition(killedAABB.center)

        local newArrowTemplate = bowComponent.arrowTemplate
        local displayText = str_utils.camelCaseToSpaces(newArrowTemplate)

        setUpdateFunction(text, .05, function()

            local textView = component.TextView.getFor(text)

            if #textView.text == 0 then

                local iconSprite = component.AsepriteView.getFor(icon)
                playAsepriteTag(iconSprite, newArrowTemplate, true)

                local iconUIEl = component.UIElement.getFor(icon)
                iconUIEl.renderOffset.x = screenKillPos.x - 42
                iconUIEl.renderOffset.y = screenKillPos.y * -1 + 16

                component.UIElement.animate(icon, "renderOffset", iconRenderOffset, 1.5, "pow3", function()
                    print("animation done")
                end)

                textView.waving = true
                textView.wavingAmplitude = 4

                setUpdateFunction(text, .03, function()

                    local textView = component.TextView.getFor(text)

                    if #textView.text == #displayText then

                        setUpdateFunction(text, .05, function()

                            local textView = component.TextView.getFor(text)

                            textView.wavingAmplitude = textView.wavingAmplitude * .9
                            if textView.wavingAmplitude < .5 then
                                textView.waving = false
                                setUpdateFunction(text, 0, nil)
                            end
                        end)
                        return
                    end

                    textView.text = displayText:sub(1, #textView.text + 1)
                end)
                return
            end

            textView.text = textView.text:sub(1, #textView.text - 1)
        end)
    end)
end