
function create(stem)

    flower = createChild(stem)

    setComponents(flower, {
        AABB = {
            halfSize = {3, 3}
        },
        AsepriteView = {
            sprite = "sprites/plant",
            frame = math.random(0, 2)
        }
    })

    applyTemplate(stem, "Rope", {
        draw = true,
        color = colors.grass_dark,
        moveByWind = 10
    })

    length = math.random(5, 25)

    setComponent(stem, "VerletRope", {
        endPointEntity = flower,
        length = length,
        gravity = {math.random(-3, 3), math.random(10, 30)},
        friction = math.random(77, 99) / 100,
        nrOfPoints = 4
    })

    leavesI = length

    while leavesI > 7 do

        leavesI = leavesI - math.random(7, 10)

        setComponents(createChild(stem), {
            AABB = {},
            AttachToRope = {
                ropeEntity = stem,
                x = .8 - (leavesI / length)
            },
            AsepriteView = {
                sprite = "sprites/plant",
                frame = math.random(3, 5)
            }
        })

    end

end

