
function create(hud)

    setName(hud, "hud_border")

    setComponents(hud, {
        UIElement {
            absolutePositioning = true
        },
        UIContainer {
            nineSliceSprite = "sprites/ui/hud_border",
            fillRemainingParentHeight = true,
            fillRemainingParentWidth = true,

            zIndexOffset = -10
        }
    })

    currentEngine.showRetryButton = function()

        local btn = createEntity()
        setName(btn, "retry button")
        applyTemplate(btn, "Button", {
            text = "[R] Retry",
            action = function()
                print("lol")

            end
        })
        local btnUI = component.UIElement.getFor(btn)
        btnUI.absolutePositioning = true
        btnUI.absoluteHorizontalAlign = 2
        btnUI.absoluteVerticalAlign = 2
        btnUI.renderOffset = ivec2(-70, 30)
    end

    setOnDestroyCallback(hud, function()
        currentEngine.showRetryButton = nil
    end)
end

