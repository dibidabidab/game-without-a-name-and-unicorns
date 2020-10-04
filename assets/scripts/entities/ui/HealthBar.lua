

defaultArgs({
    entityRoom = nil,
    entity = nil
})


function create(hpBar, args)

    setComponents(hpBar, {
        UIElement {
            absolutePositioning = true,
            absoluteHorizontalAlign = 1,
            absoluteVerticalAlign = 2
        },
        UIContainer {
            fixedHeight = 12,
            centerAlign = true
        }
    })

    local text = createChild(hpBar)
    applyTemplate(text, "Text", {
        color = colors.brick
    })

    args.entityRoom.onEvent("BeforeDelete", function()
        if valid(hpBar) then -- todo, move this check to destroyEntity() and similar functions?
            destroyEntity(hpBar)
        end
    end)

    local prevHealth = nil
    local prevMaxHealth = nil

    setUpdateFunction(hpBar, 0, function()

        if not args.entityRoom.valid(args.entity) then
            destroyEntity(hpBar)
            return
        end

        local currHealth = 0
        local maxHealth = prevMaxHealth

        local healthComp = args.entityRoom.component.Health.tryGetFor(args.entity)
        if healthComp ~= nil then
            currHealth = healthComp.currHealth
            maxHealth = healthComp.maxHealth
        end

        if currHealth == prevHealth and maxHealth == prevMaxHealth then
            return
        end

        local textView = component.TextView.getFor(text)
        textView.text = "HP: "..tostring(currHealth).."/"..tostring(maxHealth)

        prevHealth = currHealth
        prevMaxHealth = maxHealth
    end)
end

