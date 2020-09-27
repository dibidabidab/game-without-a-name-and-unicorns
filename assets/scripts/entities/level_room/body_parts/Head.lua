
defaultArgs({
    body = nil,
    halfSize = ivec2(2, 4),
    sprite = "sprites/player_head"
})

function create(head, args)
    components = {
        Head {
            body = args.body
        },
        AABB {
            halfSize = args.halfSize
        },
        AsepriteView {
            sprite = args.sprite
        },
        SpriteAnchor {
            spriteEntity = args.body,
            spriteSliceName = "head"
        }
    }
    setComponents(head, components)
end
