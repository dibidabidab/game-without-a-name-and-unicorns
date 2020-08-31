

function create(particle)

    spawnedBy = getComponent(particle, "SpawnedBy")
    rainbow = spawnedBy.spawner
    line = getComponent(rainbow, "Polyline")

    x = spawnedBy.spawnerPos[1]
    y = spawnedBy.spawnerPos[2]

    pointIndex = 1
    if math.random() > .5 then
        pointIndex = #line.points
    end

    x = x + line.points[pointIndex][1]
    y = y + line.points[pointIndex][2] + math.random(-12, 0)

    maxVel = 60

    components = {
        AABB = {
            center = {x, y},
            halfSize = {math.random(1, 5), math.random(1, 5)}
        },
        Physics = {
            gravity = 0,
            velocity = {math.random(-maxVel, maxVel), math.random(-maxVel, maxVel)}
        },
        DynamicCollider = {},
        DespawnAfter = {
            time = 1.
        },
        AsepriteView = {
            sprite = "sprites/rainbow_particle",
            loop = false,
            playingTag = 0,
        }
    }

    setComponents(particle, components)

end


