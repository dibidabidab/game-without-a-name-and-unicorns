
function create(widget)

    setComponents(widget, {
        UIElement {
            margin = ivec2(16),
            absolutePositioning = true,
            absoluteHorizontalAlign = 2 -- todo, use strings or constants
        },
        UIContainer {
            nineSliceSprite = sprite_asset("sprites/ui/default_9slice_plane"),
            autoHeight = true,
            --autoWidth = true,
            fixedWidth = 128,
            centerAlign = true
        },
        Inspecting()
    })

    applyTemplate(createChild(widget), "Text", {
        text = "dibidabidab!!!\n\n\nlol",
        color = colors.brick,
        waving = true
    })

    setComponents(createChild(widget), {
        UIElement(),
        AsepriteView {
            sprite = sprite_asset("sprites/enemy")
        }
    })
    applyTemplate(createChild(widget), "Text", {
        text = "HP: 24/30"
    })


    local button = createChild(widget, "bttn")
    setComponents(button, {
        UIElement {
            startOnNewLine = true
        },
        UIContainer {
            nineSliceSprite = sprite_asset("sprites/ui/default_9slice_plane"),
            fixedWidth = 100,
            fixedHeight = 24,
            fillRemainingParentHeight = false,
            padding = ivec2(0, -5),
            centerAlign = true
        }
    })
    applyTemplate(createChild(button), "Text", {
        text = "OK"
    })


end
