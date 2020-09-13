
function create(widget)

    setComponents(widget, {
        UIElement = {},
        UIContainer = {
            nineSliceSprite = "sprites/ui/default_9slice_plane",
            autoHeight = true
        },
        Inspecting = {}
    })

    applyTemplate(createChild(widget), "Text", {
        text = "dibidabidab!!!\n\n\nlol",
        color = colors.brick,
        waving = true
    })

    setComponents(createChild(widget), {
        UIElement = {},
        AsepriteView = {
            sprite = "sprites/enemy"
        }
    })
    applyTemplate(createChild(widget), "Text", {
        text = "HP: 24/30"
    })

end
