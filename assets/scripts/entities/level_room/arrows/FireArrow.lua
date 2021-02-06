
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
        Light {
            radius = 16
        }
    })
end
