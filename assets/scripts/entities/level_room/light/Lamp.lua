function create(rope)

    local lamp = createChild(rope, "lamp")

    applyTemplate(rope, "Rope", {
        draw = true,
        color = colors.wood,
        endPointEntity = lamp,
        length = math.random(20, 40),
        moveByWind = 1,
        updatePrecision = 4
    })

    setComponents(lamp, {
        AABB {
            halfSize = ivec2(3, 8)
        },
        PointLight {
            radius = math.random(60, 90),
            checkForTerrainCollision = true,
            radiusFlickeringFrequency = 3.,
            radiusFlickeringIntensity = 3
        },
        AsepriteView {
            sprite = "sprites/lamp",
            playingTag = 0,
            frame = math.random(10),
            aabbAlign = vec2(.5, 0)
        },
        DynamicCollider(),
        Health {
            takesDamageFrom = {"hit"},
            currHealth = 1,
            maxHealth = 1,
            bloodColor = 0
        }
    })

    onEntityEvent(lamp, "Died", function ()

        component.PointLight.remove(lamp)
        component.SliceSpriteIntoPieces.getFor(lamp).steps = 3

        setComponent(lamp, SoundSpeaker {
            sound = "sounds/glass_break",
            pitch = math.random() * .6 + .7,
            volume = math.random() * .3 + .5
        }, PositionedAudio())
    end)
end
