
defaultArgs({
    icon = nil
})

function create(item, args)

    setComponents(item, {
        UIElement {
            absolutePositioning = true,
            absoluteHorizontalAlign = 0,
            absoluteVerticalAlign = 2,
            renderOffset = ivec2(22, 64)
        },
        UIContainer {
            nineSliceSprite = "sprites/ui/default_9slice_plane",
            fixedHeight = 27,
            fixedWidth = 27,
            centerAlign = true
        }
    })
    setComponents(createChild(item, "icon"), {
        UIElement {
            renderOffset = ivec2(0, 0)
        },
        AsepriteView {
            sprite = args.icon
        }
    })

    component.UIElement.animate(item, "renderOffset", ivec2(22, 18), .2, "pow3Out")
end
