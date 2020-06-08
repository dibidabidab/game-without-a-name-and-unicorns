
-- a simple template that makes it easy to create a SpriteAnchor

arg("spriteEntity", nil)
arg("sliceName", "anchor")
arg("ignoreSpriteFlipping", false)

components = {
    SpriteAnchor = {
        spriteEntity = args.spriteEntity,
        spriteSliceName = args.sliceName,
        ignoreSpriteFlipping = args.ignoreSpriteFlipping
    },
    AABB = {}
}
