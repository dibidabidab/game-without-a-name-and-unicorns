
function create(widget)

    setComponents(widget, {
        UIElement = {
            margin = {16, 16},
            absolutePositioning = true,
            absoluteHorizontalAlign = 2
        },
        UIContainer = {
            nineSliceSprite = "sprites/ui/default_9slice_plane",
            autoHeight = true,
            --autoWidth = true,
            fixedWidth = 128,
            centerAlign = true
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


    local button = createChild(widget, "bttn")
    setComponents(button, {
        UIElement = {
            startOnNewLine = true
        },
        UIContainer = {
            nineSliceSprite = "sprites/ui/default_9slice_plane",
            fixedWidth = 64,
            fixedHeight = 29,
            centerAlign = true
        }
    })
    applyTemplate(createChild(button), "Text", {
        text = "OK"
    })


end
