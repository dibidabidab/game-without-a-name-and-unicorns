
-- handBowAnchor marks the position where one of the player's hand should go when holding the bow
applyTemplate(createChild("handBowAnchor"), "SpriteAnchor", {
  spriteEntity = entity,
  sliceName = "hand_bow_anchor"
})

-- handStringAnchor marks the position where one of the player's hand should go when holding the bow's string
applyTemplate(createChild("handStringAnchor"), "SpriteAnchor", {
  spriteEntity = entity,
  sliceName = "hand_string_anchor"
})

components = {
  AABB = {
    halfSize = {3, 3}
  },
  AsepriteView = {
    sprite = "sprites/bow",
    playingTag = 0,
    paused = true,
    loop = false
  },
  Bow = {
    handBowAnchor = handBowAnchor,
    handStringAnchor = handStringAnchor,

    arrowTemplate = "Arrow"
  }
}
