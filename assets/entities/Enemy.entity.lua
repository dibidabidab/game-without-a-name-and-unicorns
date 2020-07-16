
components = {
    AABB = {
        halfSize = {5, 8}
    },
    Physics = {
        ignorePolyPlatforms = false
    },
    StaticCollider = {},
    Health = {
        takesDamageFrom = {"hit"},
        componentsToAddOnDeath = {
            SliceSpriteIntoPieces = {
                steps = 6
            }
        },
        currHealth = 2,
        maxHealth = 2
    },
    AsepriteView = {
        sprite = "sprites/enemy"
    }
}

