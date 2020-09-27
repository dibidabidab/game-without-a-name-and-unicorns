
description("yes you can do a moonwalk on rainbows")

function create(rainbow)
    components = {
        AABB(),
        PolyPlatform(),
        PolylineWaves {
            impactMultiplier = 1.5
        },
        Polyline {
            points = {}
        },
        DrawPolyline {
            -- each rainbow color twice:
            repeatColors = {
                colors.rainbow_red, colors.rainbow_red,
                colors.rainbow_orange, colors.rainbow_orange,
                colors.rainbow_yellow, colors.rainbow_yellow,
                colors.rainbow_green, colors.rainbow_green,
                colors.rainbow_blue, colors.rainbow_blue,
                colors.rainbow_pink, colors.rainbow_pink
            },
            repeatY = 11,
            addAABBOffset = true,
            zIndexEnd = -64, zIndexBegin = -64
        },
        Rainbow(),
        TemplateSpawner {
            templateName = "RainbowParticle",
            minDelay = .1,
            maxDelay = .2,
            minQuantity = 3,
            maxQuantity = 10
        },
        PaletteSetter {
            paletteName = "test",
            priority = 10
        }
    }
    setComponents(rainbow, components)
end


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
