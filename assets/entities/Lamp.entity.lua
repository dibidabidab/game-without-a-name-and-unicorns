
createChild("rope")
applyTemplate(rope, "Rope", {
  draw = true,
  color = 6,
  endPointEntity = entity,
  length = math.random(20, 40)
})

components = {
  AABB = {
    halfSize = {3, 8}
  },
  Physics = {
    gravity = 0,
    ghost = true,
    moveByWind = 300
  },
  LightPoint = {
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
      }
    },
    componentsToRemoveOnDeath = {"LightPoint"},
    currHealth = 1,
    maxHealth = 1,
    bloodColor = 0
  }
}
