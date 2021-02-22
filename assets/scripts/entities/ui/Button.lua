
defaultArgs({
    text = "giv mony",
    action = nil
})

function create(btn, args)

    setComponents(btn, {
        UIElement(),
        UIContainer {
            nineSliceSprite = "sprites/ui/button",
            fixedHeight = 20,
            fixedWidth = 32,
            autoWidth = true,
            padding = ivec2(0, -2)
        },

        UIMouseEvents()
    })
    applyTemplate(createChild(btn, "text"), "Text", {
        text = args.text,
        font = "sprites/ui/default_font_no_shadow",
        color = 1
    })

    local hovered = false

    function hover(onTop)
        if not onTop then
            return
        end
        hovered = true
        component.UIContainer.animate(btn, "spriteFrame", 3, .03)
        component.UIContainer.animate(btn, "padding", ivec2(0, -1), .03)
    end
    onEntityEvent(btn, "Hover", hover)

    function hoverLeave(onTop)
        if not onTop then
            return
        end
        hovered = false
        component.UIContainer.animate(btn, "spriteFrame", 0, .05)
        component.UIContainer.animate(btn, "padding", ivec2(0, -2), .05)
    end
    onEntityEvent(btn, "HoverLeave", hoverLeave)

    onEntityEvent(btn, "Click", function(onTop)
        if not onTop then
            return
        end

        component.UIContainer.getFor(btn).spriteFrame = 4
        component.UIContainer.getFor(btn).padding = ivec2(0, 0)
    end)
    onEntityEvent(btn, "ClickReleased", function(onTop)
        if hovered then
            hover(onTop)
        else
            hoverLeave(onTop)
        end
        if args.action ~= nil then
            args.action()
        end
    end)

end
