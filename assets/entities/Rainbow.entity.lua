
components = {
    AABB = {
        center = {120, 120}
    },
    PolyPlatform = {},
    PolyPlatformWaves = {
        impactMultiplier = 1.2
    },
    Polyline = {
        points = {}
    },
    DrawPolyline = {
        -- each rainbow color twice:
        repeatColors = {10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15},
        repeatY = 11,
        addAABBOffset = true,
        zIndexEnd = -64, zIndexBegin = -64
    },
    Rainbow = {}
}

-- example rainbow:
--velX = 8
--velY = 10
--
--posX = -75
--posY = -100
--
--for _ = 1, 25 do
--
--    table.insert(components.Polyline.points, {posX, posY})
--
--    posX = posX + velX
--    posY = posY + velY
--
--    velY = velY - .8
--end
