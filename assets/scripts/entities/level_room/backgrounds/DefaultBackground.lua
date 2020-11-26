
persistenceMode(TEMPLATE)

function create(bg)

    setComponents(bg, {

        ModelView {
            modelCollection = "models/default_bg",
            modelName = "bg",
            locationOffset = vec3(450, 62, -530),
            renderWithPerspectiveCamera = true
        },
        --Inspecting()

    })

end

