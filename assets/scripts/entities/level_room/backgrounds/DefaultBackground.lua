
persistenceMode(TEMPLATE)

function create(bg)

    local rot = quat:new()
    rot.y = -42

    setComponents(bg, {

        ModelView {
            modelCollection = "models/default_bg",
            modelName = "bg",
            locationOffset = vec3(325, -10, -850),
            scale = vec3(3),
            rotation = rot,
            renderWithPerspectiveCamera = true
        },
        Inspecting()

    })

end

