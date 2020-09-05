
function create(widget)

    setComponents(widget, {
        UIElement = {},
        UIContainer = {
            nineSliceSprite = "sprites/ui/default_9slice_plane"
        },
        Inspecting = {}
    })

    applyTemplate(createChild(widget), "Text", {
        text = "dibidabidab!",
        color = colors.rainbow_red,
        waving = true
    })

    setComponents(createChild(widget), {
        UIElement = {},
        AsepriteView = {
            sprite = "sprites/enemy"
        }
    })
    applyTemplate(createChild(widget), "Text", {
        text = "heheh"
    })

end
