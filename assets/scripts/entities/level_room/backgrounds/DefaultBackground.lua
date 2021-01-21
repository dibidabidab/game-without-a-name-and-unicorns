
persistenceMode(TEMPLATE)

function create(bg)

    local rot = quat()
    rot.y = -42

    setComponents(bg, {

        ModelView {
            modelCollection = model_collection_asset("models/default_bg"),
            modelName = "bg",
            locationOffset = vec3(325, -10, -850),
            scale = vec3(3),
            rotation = rot,
            renderWithPerspectiveCamera = true
        },
        --Inspecting()

    })

end

