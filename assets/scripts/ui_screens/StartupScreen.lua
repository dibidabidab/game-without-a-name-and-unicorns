
fastStartup = include("scripts/fast_startup") -- .lua not needed

_G.hudScreen = currentEngine

onEvent("BeforeDelete", function()
    if _G.hudScreen == currentEngine then
        _G.hudScreen = nil
    end
end)

fastStartup.fastStartup()

