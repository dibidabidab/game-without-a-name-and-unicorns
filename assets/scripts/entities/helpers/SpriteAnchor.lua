
description("a simple template that makes it easy to create a SpriteAnchor")

defaultArgs({
    spriteEntity = nil,
    sliceName = "anchor",
    ignoreSpriteFlipping = false
})

function create(anchor, args)
    setComponents(anchor, {
        SpriteAnchor = {
            spriteEntity = args.spriteEntity,
            spriteSliceName = args.sliceName,
            ignoreSpriteFlipping = args.ignoreSpriteFlipping
        },
        AABB = {}
    })
end
