
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

    local retrying = false

    function retryLevel()

        if retrying then
            return
        end
        retrying = true

        print("retry -> reload level")

        startScreenTransition("textures/screen_transition1", "shaders/screen_transition/cutoff_texture_rainbow")
        onEvent("ScreenTransitionStartFinished", function()

            closeActiveScreen()
            openScreen("scripts/ui_screens/LevelScreen")
        end)
    end

    listenToKey(hud, gameSettings.keyInput.retryLevel, "retry_key")
    listenToGamepadButton(hud, 0, gameSettings.gamepadInput.retryLevel, "retry_key")
    onEntityEvent(hud, "retry_key_pressed", retryLevel)

    currentEngine.showRetryButton = function(showExitButton)

        if getChild(hud, "retry_btn") == nil then
            local btn = createChild(hud, "retry_btn")
            applyTemplate(btn, "Button", {
                text = "["..gameSettings.keyInput.retryLevel:getName().."] Retry",
                action = retryLevel
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

    currentEngine.levelFinished = function()

        startScreenTransition("textures/screen_transition2", "shaders/screen_transition/cutoff_texture")
        onEvent("ScreenTransitionStartFinished", function()

            closeActiveScreen()
            openScreen("scripts/ui_screens/LevelSelectScreen")
        end)
    end

    setOnDestroyCallback(hud, function()
        currentEngine.showRetryButton = nil
        currentEngine.levelFinished = nil
    end)
end

