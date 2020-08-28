
components = {
  Arrow = {},
  AABB = {},
  Physics = {
    gravity = 0,
    airFriction = 0,
    ignorePlatforms = true,
    createWind = .8
  },
  StaticCollider = {},
  AsepriteView = {
    sprite = "sprites/arrow"
  },

  -- the white trail you see when shooting an arrow:
  Polyline = {},
  DrawPolyline = {
    colors = { 7 }
  },
  TransRoomable = {
    templateName = "" -- do not create a new Arrow, just let this one disappear.
  }
}
