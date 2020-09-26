
p = Physics:new {
    gravity = 45,
    velocity = vec2(22, 313.443)
}

print(p.velocity.x, p.velocity.y)

assert(p.wallFriction == 6)
assert(p.gravity == 45)



fastStartup = include("scripts/fast_startup") -- .lua not needed

fastStartup.fastStartup()

--applyTemplate(createEntity(), "TestUIWidget")
applyTemplate(createEntity(), "Hud")
