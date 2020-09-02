function create(arrow)

    applyTemplate(arrow, "Arrow")

    setComponents(arrow, {
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
    })

end
