
persistenceMode(TEMPLATE | FINAL_POS) -- FINAL_POS also supports ModelView.locationOffset

function create(bg)

    setName(bg, "3d_background")

    local rot = quat:new()
    rot.y = -42

    pos = component.ModelView.tryGetFor(bg)
    if pos then
        pos = pos.locationOffset
    else
        pos = vec3(325, -10, -850)
    end

    setComponents(bg, {

        ModelView {
            modelCollection = "models/default_bg",
            modelName = "bg",
            locationOffset = pos,
            scale = vec3(3),
            rotation = rot,
            renderWithPerspectiveCamera = true
        },
        --Inspecting()

    })

end

