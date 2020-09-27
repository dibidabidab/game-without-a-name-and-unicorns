
function create(hud)

    setComponents(hud, {
        UIElement {
            absolutePositioning = true
        },
        UIContainer {
            nineSliceSprite = "sprites/ui/hud_border",
            fillRemainingParentHeight = true,
            fillRemainingParentWidth = true
        }
    })
end

