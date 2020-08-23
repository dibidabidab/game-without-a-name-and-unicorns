
applyTemplate(entity, "Arrow")

components = {
  RainbowSpawner = {
    rainbowSettings = {
      disappearOnPlayerTouch = true,
      disappearAfterTime = 5,
      disappearSpeed = 75
    }
  },
  Physics = {
    gravity = 4000,
    createWind = 1.5
  }
}
