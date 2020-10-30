

function create(particle)

    local spawnedBy = component.SpawnedBy.getFor(particle)
    local rainbow = spawnedBy.spawner
    local line = component.Polyline.getFor(rainbow)

    local x = spawnedBy.spawnerPos.x
    local y = spawnedBy.spawnerPos.y

    pointIndex = 1
    if math.random() > .5 then
        pointIndex = #line.points
    end

    x = x + line.points[pointIndex].x
    y = y + line.points[pointIndex].y + math.random(-12, 0)

    maxVel = 60

    local aabb = component.AABB.getFor(particle)
    aabb.center = ivec2(x, y)
    aabb.halfSize = ivec2(math.random(1, 5), math.random(1, 5))

    local physics = component.Physics.getFor(particle)
    physics.gravity = 0
    physics.velocity = vec2(math.random(-maxVel, maxVel), math.random(-maxVel, maxVel))

    component.DespawnAfter.getFor(particle).time = 1.

    local spriteView = component.AsepriteView.getFor(particle)
    spriteView.sprite = "sprites/rainbow_particle"
    spriteView.loop = false
    spriteView.playingTag = 0

end


