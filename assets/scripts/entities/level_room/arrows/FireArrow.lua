
function create(arrow)

    applyTemplate(arrow, "DefaultArrow")

    setComponents(arrow, {
        Fire {
            width = 1
        },
        Igniter {
            igniteChance = 1.,
            range = 16
        },
        PointLight {
            radius = 16
        }
    })
    component.Arrow.getFor(arrow).damageType = "fire"

    onEntityEvent(arrow, "TerrainHit", function()

        component.Fire.animate(arrow, "intensity", .0, 5., "linear", function()
            component.Fire.remove(arrow)
        end)
        setTimeout(arrow, 4.8, function()
            component.PointLight.animate(arrow, "radius", 0, .2, "pow2", function()
                component.PointLight.remove(arrow)
            end)
        end)
    end)
end
