
components = {
    AABB = {
        halfSize = {4, 3}
    },
    Physics = {},
    AsepriteView = {
        sprite = "sprites/apple"
    },
    Health = {
        takesDamageFrom = {"hit"},
        componentsToAddOnDeath = {
            SliceSpriteIntoPieces = {
                steps = 4
            },
            DespawnAfter = {
                time = .1
            }
        },
        currHealth = 1,
        maxHealth = 1,
        bloodColor = 0
    }
}
