lib = include("scripts/lib")

arg("ageArg", math.random(10, 100))
arg("zIndexArg", -600 - math.random(200))
arg("typeArg", "oak")

--[[ types in tree config
    length          Length in pixels
    factor          Factor of other measuree
    amount          An integer amount
    degrees         rotation
    enum            See description
    xRange          lib.range(min, max) or lib.range(max): range of length/factor/amount/degrees
                    lib.sqRange(min, max) or lib.sqRange(max) similar to range, but when getting a random value in this range,
                        the values get squared. They are still guaranteed to stay between min and max.
                    lib.[sq]rangeDev(dev) [sq]range from -dev to dev
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

treeConfigs = {
    default = {

    },
}

age         = ageArg
zIndex      = zIndexArg
type        = typeArg
rootConfig  = treeConfigs[type]

-- Component functions

function rootState      (treeType)
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

function piece          (parent, treeConfig, treeState, entity)
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

function continueBranch (parent, treeConfig, treeState)
    local angle        = lib.random(treeConfig.pieceAngle)
    local bRotation    = treeState.rotation
    treeState.rotation = lib.rotate(treeState.rotation, angle)

    piece(parent, treeConfig, treeState)

    treeState.rotation = bRotation
end

function branch(parent, treeConfig, treeState)
    local angle        = lib.random(treeConfig.branchAngle)
    doBranch(parent, treeConfig, treeState, angle)
end

function crown(parent, treeConfig, treeState)
    local amount  = lib.random(treeConfig.crownBranches)
    local angles = randomDistrMin(treeConfig.crownAngle, treeConfig.crownMinAngle, amount)
    for _, angle in pairs(angles) do
        doBranch(parent, treeConfig, treeState, angle)
    end
end

function doBranch(parent, treeConfig, treeState, angle)
    local bRotation    = treeState.rotation
    treeState.rotation = lib.rotate(treeState.rotation, angle)
    treeState.depth    = treeState.depth + 1

    piece(parent, treeConfig, treeState)

    treeState.rotation = bRotation
    treeState.depth    = treeState.depth - 1
end

function treePieces     (parent, treeConfig, treeState)
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

