
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

        component.Fire.animate(arrow, "intensity", .0, 5., function()
            component.Fire.remove(arrow)
        end)
    end)
end
