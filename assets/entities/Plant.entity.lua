
components = {
    AABB = {
        halfSize = {3, 3}
    },
    AsepriteView = {
        sprite = "sprites/plant",
        frame = math.random(0, 2)
    },
    DynamicCollider = {}
}

createChild("plantStem")
applyTemplate(plantStem, "Rope", {
    draw = true,
    color = 4,
    moveByWind = 10
})

length = math.random(5, 25)

childComponents.plantStem.VerletRope = {
    endPointEntity = entity,
    length = length,
    gravity = {math.random(-3, 3), math.random(10, 30)},
    friction = math.random(77, 99) / 100,
    nrOfPoints = 5
}

leavesI = length

while leavesI > 7 do

    leavesI = leavesI - math.random(7, 10)

    childName = "leaves"..leavesI
    createChild(childName)

    childComponents[childName] = {
        AABB = {},
        AttachToRope = {
            ropeEntity = plantStem,
            x = .8 - (leavesI / length)
        },
        AsepriteView = {
            sprite = "sprites/plant",
            frame = math.random(3, 5)
        }
    }

end
