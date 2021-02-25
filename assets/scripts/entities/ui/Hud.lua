
function create(hud)

    setName(hud, "hud")

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

    currentEngine.showRetryButton = function(showExitButton)

        if getChild(hud, "retry_btn") == nil then
            local btn = createChild(hud, "retry_btn")
            applyTemplate(btn, "Button", {
                text = "[R] Retry",
                action = function()

                    print("retry -> reload level")
                    closeActiveScreen()
                    openScreen("scripts/ui_screens/LevelScreen")
                end
            })
            local btnUI = component.UIElement.getFor(btn)
            btnUI.absolutePositioning = true
            btnUI.absoluteHorizontalAlign = 2
            btnUI.absoluteVerticalAlign = 2
            btnUI.renderOffset = ivec2(-37, 1)
        end

        if showExitButton and getChild(hud, "level_select_btn") == nil then
            local btn = createChild(hud, "level_select_btn")
            applyTemplate(btn, "Button", {
                text = "Exit",
                action = function()

                    print("back to level select")
                    closeActiveScreen()
                    openScreen("scripts/ui_screens/LevelSelectScreen")
                end
            })
            local btnUI = component.UIElement.getFor(btn)
            btnUI.absolutePositioning = true
            btnUI.absoluteHorizontalAlign = 2
            btnUI.absoluteVerticalAlign = 2
            btnUI.renderOffset = ivec2(-83, 1)
        end
    end

    setOnDestroyCallback(hud, function()
        currentEngine.showRetryButton = nil
    end)
end

