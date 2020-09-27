
description("a simple template that makes it easy to create a SpriteAnchor")

defaultArgs({
    spriteEntity = nil,
    sliceName = "anchor",
    ignoreSpriteFlipping = false
})

function create(anchor, args)

    component.AABB.getFor(anchor)
    local comp = component.SpriteAnchor.getFor(anchor)
    comp.spriteEntity = args.spriteEntity
    comp.spriteSliceName = args.sliceName
    comp.ignoreSpriteFlipping = args.ignoreSpriteFlipping

end
