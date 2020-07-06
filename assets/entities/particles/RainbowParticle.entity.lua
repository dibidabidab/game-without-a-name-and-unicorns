
rainbow = getComponent(entity, "SpawnedBy").spawner
line = getComponent(rainbow, "Polyline")
rainbowAABB = getComponent(rainbow, "AABB")

x = rainbowAABB.center[1]
y = rainbowAABB.center[2]

pointIndex = 1
if math.random() > .5 then
    pointIndex = #line.points
end

x = x + line.points[pointIndex][1]
y = y + line.points[pointIndex][2] + math.random(-12, 0)

components = {
    AABB = {
        center = {x, y}
    },
    Physics = {
        gravity = 0,
        velocity = {math.random(-40, 40), math.random(-40, 40)}
    },
    DynamicCollider = {},
    DespawnAfter = {
        time = .4
    },
    AsepriteView = {
        sprite = "sprites/plant",
        frame = math.random(0, 2)
    }
}




