
-- a simple template that makes it easy to create a SpriteAnchor

arg("spriteEntity", nil)
arg("sliceName", "anchor")

components = {
    SpriteAnchor = {
        spriteEntity = args.spriteEntity,
        spriteSliceName = args.sliceName
    },
    AABB = {}
}
