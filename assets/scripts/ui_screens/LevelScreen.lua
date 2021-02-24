
_G.hudScreen = currentEngine

onEvent("BeforeDelete", function()
    if _G.hudScreen == currentEngine then
        _G.hudScreen = nil
    end
end)

if _G.levelToLoad == nil then
    error("_G.levelToLoad is nil")
end

applyTemplate(createEntity(), "Hud")

loadOrCreateLevel(_G.levelToLoad)
