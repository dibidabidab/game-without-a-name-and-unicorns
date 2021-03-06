
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
            takesAnyDamage = true,
            currHealth = 1,
            maxHealth = 1,
            bloodColor = 0
        }
    })
    onEntityEvent(apple, "Died", function()
        setComponents(apple, {
            SliceSpriteIntoPieces {
                steps = 4
            },
            DespawnAfter {
                time = .1
            },
            SoundSpeaker {
                sound = "sounds/apple_hit",
                pitch = math.random() + .9,
                volume = 1. + math.random()
            },
            PositionedAudio()
        })
    end)
end
