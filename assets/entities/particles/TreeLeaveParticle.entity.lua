
spawnedBy = getComponent(entity, "SpawnedBy")

x = spawnedBy.spawnerPos[1]
y = spawnedBy.spawnerPos[2]

components = {
    AABB = {
        center = {x, y}
    },
    Physics = {
        gravity = math.random(30, 120),
        velocity = {
            math.random(-30, 30), 0
        },
        airFriction = .5,
        moveByWind = 300

    },
    AsepriteView = {
        sprite = "sprites/tree_leaves",
        frame = 3,
        flipHorizontal = math.random() > .5,
        flipVertical = math.random() > .5
    },
    DespawnAfter = {
        time = 3
    }
}
