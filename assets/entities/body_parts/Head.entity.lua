
arg("body", nil)
arg("halfSize", {2, 4})
arg("sprite", "sprites/player_head")

components = {
    Head = {
        body = args.body
    },
    AABB = {
        halfSize = args.halfSize
    },
    AsepriteView = {
        sprite = args.sprite
    },
    SpriteAnchor = {
        spriteEntity = args.body,
        spriteSliceName = "head"
    }
}
