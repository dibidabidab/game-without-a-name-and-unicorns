
description("i like apples")

function create(apple)

    setComponents(apple, {
        AABB {
            halfSize = ivec2(4, 3)
        },
        Physics(),
        AsepriteView {
            sprite = "sprites/apple"
        },
        Health {
            takesDamageFrom = {"hit"},
            componentsToAddOnDeath = {
                SliceSpriteIntoPieces = {
                    steps = 4
                },
                DespawnAfter = {
                    time = .1
                },
                SoundSpeaker = {
                    sound = "sounds/apple_hit",
                    pitch = math.random() + .9,
                    volume = 1. + math.random()
                }
            },
            currHealth = 1,
            maxHealth = 1,
            bloodColor = 0
        }
    })
end
