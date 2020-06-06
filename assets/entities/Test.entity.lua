
print("Test is gonna add some components to "..entity)

components = {
    Physics = {
        gravity = 12345
    },
    AABB = {
        center = {16, 16},
        halfSize = {10, 4}
    },
    LightPoint = {
        radius = 100
    },
    AsepriteView = {
        sprite = "sprites/enemy"
    }
}

createChild("lolol")

applyTemplate(lolol, "Test2", {
    size = 10
})

childComponents.lolol.Physics = {
    gravity = -10
}
