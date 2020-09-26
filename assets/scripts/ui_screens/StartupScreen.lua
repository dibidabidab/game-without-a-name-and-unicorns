
p = Physics:new {
    gravity = 45,
    velocity = 4
}

assert(p.wallFriction == 6)
assert(p.gravity == 1000)



fastStartup = include("scripts/fast_startup") -- .lua not needed

fastStartup.fastStartup()

--applyTemplate(createEntity(), "TestUIWidget")
applyTemplate(createEntity(), "Hud")
