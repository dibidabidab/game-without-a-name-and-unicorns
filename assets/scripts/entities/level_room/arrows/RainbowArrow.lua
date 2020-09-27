function create(arrow)

    applyTemplate(arrow, "Arrow")

    setComponent(arrow, RainbowSpawner {
        rainbowSettings = Rainbow {
            disappearOnPlayerTouch = true,
            disappearAfterTime = 5,
            disappearSpeed = 75
        }
    })

    local p = component.Physics.getFor(arrow)
    p.gravity = 4000
    p.createWind = 1.5

end
