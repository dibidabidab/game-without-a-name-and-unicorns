arg("ageArg", math.random(10, 100))
arg("zIndexArg", -600 - math.random(200))
arg("typeArg", "oak")

--[[ types in tree config
    length          Length in pixels
    factor          Factor of other measuree
    amount          An integer amount
    degrees         rotation
    enum            See description
    xRange          lib.range(min, max) for a range of x, where x is length, factor, amount or degrees
                    lib.sqrange(min, max) similar to range(min, max), but when getting a random value in this range,
                        the values get squared. They are still guaranteed to stay between min and max though.
]]--

--[[ TreeConfig
    length                  length      200     Max length of the stem at full age. Length at age 0 is 0
    branchlessStart         factorRange .3      Amount of branch from the start that cannot have any branches split off of it
    width                   factorRange 1       Stem width factor. 1 is normal width. Width depends on size and weight

    pieceLength             lengthRange 10      Length of tree pieces
    pieceAmount             amountRange         Amount of tree pieces in branch
    pieceAngle              degreeRange         Angle of a branch continuation piece

    branchChance            factorRange         Chance of branches splitting off
    branchAngle             degreeRange         Angle of branches splitting off

    crownBranches           amountRange         Amount of branches coming out of the crown of a branch
    crownAngle              degreeRange         Angle of branches at the crown
    crownMinAngle           degrees             Minimum angle between branches at the crown

    tendency                enum                "up", "out", "down", "none": tendency for the tree to grow towards
    tendencyStrength        factor              Strenght of tendency: 1 is medium tendency, above 1 will ignore some limits

    branchLengthFct         factor              Each branch should have a reduced length (0-.9) or not (1)
    branchPiecesFct         factor              Each branch should have a reduced nr of pieces (0-.9) or not (1)
    pieceLengthFct          factor              Each branch piece should have a reduced length (0-.9) or not (1)

    limitLength             length              Limit the total length of recursive branches
    limitDepth              amount              Limit the depth of recursive branches
    limitPieces             amount              Limit the depth of recursive branch pieces
]]--

treeConfigs    = {
    default = {
    },
}

age            = ageArg
zIndex         = zIndexArg
type           = typeArg
rootConfig     = treeConfigs[type]

lib            = {
    range          = function(min, max)
        return {
            min = min,
            max = max,
            fn  = lib.id
        }
    end,
    sqRange        = function(min, max)
        return {
            min = min,
            max = lib.psSqrt(max),
            fn  = lib.psSquare,
        }
    end,
    random         = function(range)
        return range.fn(math.random(range.min, range.max))
    end,
    cap            = function(range, value)
        if range.max < range.min then
            return lib.cap({ min = range.max, max = range.min, fn = range.fn })
        end
        if value < range.min then
            return range.min
        end
        if value > range.max then
            return range.max
        end
        return value
    end,
    interpolate    = function(from, to, value)
        value = (value - from.min) / (from.max - from.min)
        return value * (to.max - to.min) + to.min
    end,
    interpolateCap = function(from, to, value)
        return lib.interpolate(from, to, lib.cap(from, value))
    end,
    rotate         = function(rotation, angle)
        return rotate2d(rotation[1], rotation[2], angle)
    end,
    upperLimit     = function(value, threshold)
        if value == nil then return false end
        return value >= threshold
    end,
    lowerLimit     = function(value, threshold)
        if value == nil then return false end
        return value <= threshold
    end,
    psSquare       = function(value)
        return value * value * lib.sign(value)
    end,
    psSqrt         = function(value)
        return lib.sign(value) * math.sqrt(value * lib.sign(value))
    end,
    sign           = function(value)
        return select(value < 0, -1, 1)
    end,
    select         = function(selector, tt, ff)
        if selector then return tt else return ff end
    end,
    id             = function(v) return v end,
}

-- Component functions

rootState      = function(treeType)
    local treeConfig = treeConfigs[treeType]
    return {
        name         = "tree." + treeType,
        rotation     = { 0, 100 },
        zIndex       = zIndex,
        zDelta       = 0,
        length       = 0,
        depth        = 0,
        pieces       = 0,
        totalPieces  = 0,

        pieceLength  = treeConfig.pieceLength,
        pieceAmount  = treeConfig.pieceAmount,
        branchLength = treeConfig.branchLength
    }
end

piece          = function(parent, treeConfig, treeState, entity)
    entity                = entity or createChild(parent, "branch")

    local length          = lib.random(treeState.pieceLength)

    treeState.length      = treeState.length + length
    treeState.pieces      = treeState.pieces + 1
    treeState.totalPieces = treeState.totalPieces + 1

    local width           = treePieces(entity, treeConfig, stateTable)

    treeState.length      = treeState.length + length
    treeState.pieces      = treeState.pieces - 1
    treeState.totalPieces = treeState.totalPieces - 1
end

continueBranch = function(parent, treeConfig, treeState)

end

treePieces     = function(parent, treeConfig, treeState)
    -- Check if branch piece limits have been reached.
    if lib.upperLimit(treeConfig.limitLength, treeState.length) or
            lib.upperLimit(treeConfig.limitPieces, treeState.totalPieces) then
        return 0
    end

    -- Check if this branch has finished.
    if not lib.upperLimit(treeConfig.pieceAmount, treeState.pieces) then
        -- Not finished, continue branch
        continueBranch(parent, treeConfig, treeState)
        if not lib.upperLimit(treeConfig.limitDepth, treeState.depth) then
            -- Create new branch from this point, if depth limit has not been reached
            branch(parent, treeConfig, treeState)
        end
    else if not lib.upperLimit(treeConfig.limitDepth, treeState.depth) then
        --Finished, create crown if depth limit has not been reached
        crown(parent, treeConfig, treeState)
    end
    end
end

