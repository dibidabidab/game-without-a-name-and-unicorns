
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
  LightPoint = {
    radius = math.random(60, 90)
  },
  AsepriteView = {
    sprite = "sprites/lamp",
    playingTag = 0,
    frame = math.random(10),
    aabbAlign = {.5, 0}
  },
  DynamicCollider = {}
}
