mtlib        = include("scripts/util/math")
treeConfigs  = include("scripts/entities/level_room/foliage/_Tree.config")

branchSystem = include("scripts/entities/level_room/foliage/_Tree.branches")

function resetDefaults()
    defaultArgs({
                    zIndex = -500 + math.random(100),
                    age    = 100, --mtlib.random(treeConfigs.ageRange),
                    seed   = math.random(math.random(10000000)),
                    type   = "oak",
                })
end


-- Component functions

function rootState      (treeType, zIndex, age)
    local treeConfig        = treeConfigs[treeType]
    local tendencyDirection = mtlib.select(treeConfig.tendency == "direction", treeConfig.tendencyDirection, 0)
    return {
        name              = treeType,
        age               = age,
        angle             = 0,
        zIndex            = zIndex,
        zDelta            = 0,
        length            = 0,
        totalLength       = 0,
        depth             = 0,
        pieces            = 0,
        totalPieces       = 0,
        baseWidth         = 0,

        branchlessStart   = mtlib.random(treeConfig.branchlessStart),
        leavesStart       = mtlib.random(treeConfig.leavesStart),

        pieceLength       = treeConfig.pieceLength,
        pieceAmount       = treeConfig.pieceAmount,
        branchLength      = treeConfig.branchLength,

        tendencyDirection = tendencyDirection,
        tendencyStrength  = treeConfig.tendencyStrength,
    }
end

function create(entity, args)

    local spawnPos = ivec2()
    local aabb = component.AABB.tryGetFor(entity)
    if aabb ~= nill then
        spawnPos = ivec2(aabb.center.x, aabb.center.y)
    end

    mtlib.seed(args.seed)
    local treeState      = rootState(args.type, args.zIndex, args.age)
    local treeConfig     = treeConfigs[args.type]
    treeState.rootEntity = entity
    treeState.baseWidth  = branchSystem.piece(nil, treeConfig, treeState, entity)

    -- Tree root should not be attatched to a parent
    component.AttachToRope.remove(entity)

    -- Grass at the bottom of the tree
    setComponent(entity, AsepriteView {
        sprite = sprite_asset("sprites/tree_leaves"),
        frame  = 4,
        zIndex = args.zIndex
    })

    -- Larger AABB, so we can pick it up easier
    aabb = component.AABB.getFor(entity)
    aabb.halfSize = ivec2(4)
    aabb.center.x = spawnPos.x
    aabb.center.y = spawnPos.y

    resetDefaults()
end

resetDefaults()