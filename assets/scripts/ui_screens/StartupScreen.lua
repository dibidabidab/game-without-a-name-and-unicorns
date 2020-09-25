
p = Physics:new {
    gravity = 1000
}

assert(p.gravity == 1000)
assert(p.wallFriction == 6)





fastStartup = include("scripts/fast_startup") -- .lua not needed

fastStartup.fastStartup()

--applyTemplate(createEntity(), "TestUIWidget")
applyTemplate(createEntity(), "Hud")
