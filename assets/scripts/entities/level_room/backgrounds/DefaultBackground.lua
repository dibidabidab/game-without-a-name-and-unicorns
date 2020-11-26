
persistenceMode(TEMPLATE)

function create(bg)

    setComponents(bg, {

        ModelView {
            modelCollection = "models/default_bg",
            modelName = "bg"
        },
        Inspecting()

    })

end

