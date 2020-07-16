
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
        }
    },
    AsepriteView = {
        sprite = "sprites/enemy"
    }
}
