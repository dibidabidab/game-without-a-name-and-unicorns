function create(rope)

    lamp = createChild(rope, "lamp")

    applyTemplate(rope, "Rope", {
      draw = true,
      color = colors.wood,
      endPointEntity = lamp,
      length = math.random(20, 40),
      moveByWind = 1,
      updatePrecision = 4
    })

    components = {
      AABB = {
        halfSize = {3, 8}
      },
      PointLight = {
        radius = math.random(60, 90),
        checkForTerrainCollision = true,
        radiusFlickeringFrequency = 3.,
        radiusFlickeringIntensity = 3
      },
      AsepriteView = {
        sprite = "sprites/lamp",
        playingTag = 0,
        frame = math.random(10),
        aabbAlign = {.5, 0}
      },
      DynamicCollider = {},
      Health = {
        takesDamageFrom = {"hit"},
        componentsToAddOnDeath = {
          SliceSpriteIntoPieces = {
            steps = 3
          },
          SoundSpeaker = {
            sound = "sounds/glass_break",
            pitch = math.random() * .6 + .7,
            volume = math.random() * .3 + .5
          }
        },
        componentsToRemoveOnDeath = {"PointLight"},
        currHealth = 1,
        maxHealth = 1,
        bloodColor = 0
      }
    }
    setComponents(lamp, components)

end
