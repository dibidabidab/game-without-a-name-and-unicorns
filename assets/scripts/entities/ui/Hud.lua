
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

                    component.UIMouseEvents.remove(btn)

                    startScreenTransition("textures/screen_transition0", "shaders/screen_transition/cutoff_texture")
                    onEvent("ScreenTransitionStartFinished", function()

                        closeActiveScreen()
                        openScreen("scripts/ui_screens/LevelScreen")
                    end)
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

                    component.UIMouseEvents.remove(btn)

                    print("back to level select")

                    startScreenTransition("textures/screen_transition0", "shaders/screen_transition/cutoff_texture")
                    onEvent("ScreenTransitionStartFinished", function()

                        closeActiveScreen()
                        openScreen("scripts/ui_screens/LevelSelectScreen")
                    end)
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

