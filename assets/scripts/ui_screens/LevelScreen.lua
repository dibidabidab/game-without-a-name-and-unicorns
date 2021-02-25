
_G.hudScreen = currentEngine

onEvent("BeforeDelete", function()
    loadOrCreateLevel(nil)
    if _G.hudScreen == currentEngine then
        _G.hudScreen = nil
    end
end)

if _G.levelToLoad == nil then
    error("_G.levelToLoad is nil")
end

endScreenTransition("textures/screen_transition0", "shaders/screen_transition/cutoff_texture")

applyTemplate(createEntity(), "Hud")

loadOrCreateLevel(_G.levelToLoad)
