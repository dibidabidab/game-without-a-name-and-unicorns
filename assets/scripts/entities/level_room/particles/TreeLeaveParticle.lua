
function create(particle)

    spawnedBy = getComponent(particle, "SpawnedBy")

    components = {
        AABB {
            center = spawnedBy.spawnerPos
        },
        Physics {
            gravity = math.random(30, 120),
            velocity = vec2(
                    math.random(-30, 30), 0
            ),
            airFriction = .5,
            moveByWind = 300

        },
        AsepriteView {
            sprite = sprite_asset("sprites/tree_leaves"),
            frame = 3,
            flipHorizontal = math.random() > .5,
            flipVertical = math.random() > .5
        },
        DespawnAfter {
            time = 3
        }
    }

    setComponents(particle, components)
end
