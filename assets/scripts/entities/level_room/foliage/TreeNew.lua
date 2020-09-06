mtlib        = include("scripts/util/math")
treeConfigs  = include("scripts/tmp/Tree.config")

branchSystem = include("scripts/tmp/Tree.branches")

defaultArgs({
                zIndex = -600 - math.random(200),
                age    = 1,
                seed   = math.random(math.random(1000000000)),
                type   = "oak",
            })


-- Component functions

function rootState      (treeType, zIndex, age)
    local treeConfig = treeConfigs[treeType]
    return {
        name         = treeType,
        age          = age,
        rotation     = { 0, 100 },
        zIndex       = zIndex,
        zDelta       = 0,
        length       = 0,
        totalLength  = 0,
        depth        = 0,
        pieces       = 0,
        totalPieces  = 0,
        baseWidth    = 0,

        pieceLength  = treeConfig.pieceLength,
        pieceAmount  = treeConfig.pieceAmount,
        branchLength = treeConfig.branchLength,
    }
end

function create(entity, args)
    mtlib.seed(args.seed)
    local treeState      = rootState(args.type, args.zIndex, args.age)
    local treeConfig     = treeConfigs[args.type]
    treeState.rootEntity = entity
    treeState.baseWidth  = branchSystem.piece(nil, treeConfig, treeState, entity)

    -- Tree root should not be attatched to a parent
    removeComponent(entity, "AttachToRope")

    -- Grass at the bottom of the tree
    setComponent(entity, "AsepriteView", {
        sprite = "sprites/tree_leaves",
        frame  = 4,
        zIndex = args.zIndex
    })

    -- Larger AABB, so we can pick it up easier
    setComponent(entity, "AABB", {
        halfSize = { 4, 4 }, -- vierkantje is 6x6 pixels
    })
end
