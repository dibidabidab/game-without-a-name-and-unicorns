
defaultArgs({
    arrowTemplate = "DefaultArrow",
    archer = nil,
    leftArm = nil,
    rightArm = nil,
    rotatePivot = ivec2(),
    distanceFromArcher = 9
})

function create(bow, args)

    -- handBowAnchor marks the position where one of the player's hand should go when holding the bow
    handBowAnchor = createChild(bow, "handBowAnchor")
    applyTemplate(handBowAnchor, "SpriteAnchor", {
        spriteEntity = bow,
        sliceName = "hand_bow_anchor"
    })

    -- handStringAnchor marks the position where one of the player's hand should go when holding the bow's string
    handStringAnchor = createChild(bow, "handStringAnchor")
    applyTemplate(handStringAnchor, "SpriteAnchor", {
        spriteEntity = bow,
        sliceName = "hand_string_anchor"
    })

    setComponents(bow, {
        AABB {
            halfSize = ivec2(3)
        },
        AsepriteView {
            sprite = "sprites/bow",
            playingTag = 0,
            paused = true,
            loop = false
        },
        Bow {
            handBowAnchor = handBowAnchor,
            handStringAnchor = handStringAnchor,

            archer = args.archer,
            archerLeftArm = args.leftArm,
            archerRightArm = args.rightArm,
            rotatePivot = args.rotatePivot,
            distanceFromArcher = args.distanceFromArcher,

            arrowTemplate = args.arrowTemplate
        }
    })
end
