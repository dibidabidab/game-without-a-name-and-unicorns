
function create(arrow)

    applyTemplate(arrow, "DefaultArrow")

    component.Arrow.getFor(arrow).damageType = "water"
    component.DrawPolyline.getFor(arrow).colors = {colors.water0}

    onEntityEvent(arrow, "Launched", function(angle)
        setComponents(arrow, {
            Sprinkler {
                minDropSize = .2,
                maxDropSize = 2,
                dropColor = colors.water0,
                angle = angle - 90,
                angleRange = 10,
                force = 600,
                amount = 30,
                gravity = 900
            }
        })
    end)

    onEntityEvent(arrow, "Reflected", function()
        local spr = component.Sprinkler.getFor(arrow)
        spr.angle = spr.angle - 180
        spr.amount = spr.amount * .2
    end)

    onEntityEvent(arrow, "TerrainHit", function()
        component.Sprinkler.remove(arrow)
    end)
end
