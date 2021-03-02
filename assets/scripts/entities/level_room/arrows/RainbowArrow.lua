function create(arrow)

    applyTemplate(arrow, "DefaultArrow")

    setComponent(arrow, RainbowSpawner {
        rainbowSettings = Rainbow {
            disappearOnPlayerTouch = true,
            disappearAfterTime = 5,
            disappearSpeed = 75
        }
    })

    local p = component.Physics.getFor(arrow)
    p.gravity = 4000
    p.createWind = 1.5

    local door = getByName("EndDoor")

    if door ~= nil then

        setUpdateFunction(arrow, .08, function()

            if not valid(door) then
                return
            end
            local doorAABB = component.AABB.getFor(door)
            local AABB = component.AABB.getFor(arrow)

            if vec2(AABB.center - doorAABB.center):length() < 64 then
                component.Sprinkler.remove(door)
            end
        end)
    end
    onEntityEvent(arrow, "TerrainHit", function()
        setUpdateFunction(arrow, 99, nil)
    end)
end
