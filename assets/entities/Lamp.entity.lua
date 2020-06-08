
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
    radius = 128
  },
  AsepriteView = {
    sprite = "sprites/lamp"
  },
  DynamicCollider = {}
}
