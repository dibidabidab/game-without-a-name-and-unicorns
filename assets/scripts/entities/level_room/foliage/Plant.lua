
function create(stem)

    flower = createChild(stem)

    setComponents(flower, {
        AABB {
            halfSize = ivec2(3)
        },
        AsepriteView {
            sprite = sprite_asset("sprites/plant"),
            frame = math.random(0, 2)
        }
    })

    applyTemplate(stem, "Rope", {
        draw = true,
        color = colors.grass_dark,
        moveByWind = 10
    })

    length = math.random(5, 25)

    local stemRope = component.VerletRope.getFor(stem)
    stemRope.endPointEntity = flower
    stemRope.length = length
    stemRope.gravity = vec2(math.random(-3, 3), math.random(10, 30))
    stemRope.friction = math.random(77, 99) / 100
    stemRope.nrOfPoints = 4

    leavesI = length

    while leavesI > 7 do

        leavesI = leavesI - math.random(7, 10)

        setComponents(createChild(stem), {
            AABB(),
            AttachToRope {
                ropeEntity = stem,
                x = .8 - (leavesI / length)
            },
            AsepriteView {
                sprite = sprite_asset("sprites/plant"),
                frame = math.random(3, 5)
            }
        })

    end

end

