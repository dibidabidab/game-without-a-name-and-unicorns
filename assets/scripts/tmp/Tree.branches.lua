mtlib       = include("scripts/util/math")
treeConfigs = include("scripts/tmp/Tree.config")

function entityTable(pieceEntity, parent, rotation, zIndex)

    setComponents(pieceEntity, {
        AABB         = {},

        DrawPolyline = {
            colors      = { colors.wood },
            repeatX     = 0,
            zIndexBegin = zIndex.min,
            zIndexEnd   = zIndex.max,
        },

        VerletRope   = {
            length     = 0,
            gravity    = rotation,

            friction   = .7,
            nrOfPoints = 1,
            moveByWind = 6,
        },

        AttachToRope = {
            ropeEntity = parent,
            x          = 1,
        }
    })
end

function updateFunction(width, length, treeState)
    local lengthRange = mtlib.range(length)
    local widthRange  = mtlib.range(width)

    return function(dTime, entity, _)
        local growthStart = mtlib.interpolate(mtlib.range(treeState.baseWidth), treeConfigs.ageRange,
                                              math.sqrt(treeState.baseWidth) - math.sqrt(width))
        local growthRange = mtlib.range(growthStart, treeConfigs.ageRange.max)
        currentLength = mtlib.interpolateCap(growthRange, lengthRange, treeState.age)
        currentWidth  = mtlib.interpolateCap(growthRange, widthRange, treeState.age)

        -- TODO: remove in new physics
        if currentLength < 1 then currentLength = 1 end

        setComponent(entity, "VerletRope", {
            length = currentLength
        })
        setComponent(entity, "DrawPolyline", {
            repeatX = math.sqrt(currentWidth),
        })

        if treeState.age >= 100 then
            setUpdateFunction(entity, 0, nil)
            return
        end

        if treeState.rootEntity == entity then
            treeState.age = treeState.age + dTime * 10
        end
    end
end

function piece          (parent, treeConfig, treeState, entity)
    entity                = entity or createChild(parent, "branch")

    local length          = mtlib.random(treeState.pieceLength)

    treeState.length      = treeState.length + length
    treeState.totalLength = treeState.totalLength + length
    treeState.pieces      = treeState.pieces + 1
    treeState.totalPieces = treeState.totalPieces + 1
    mtlib.rangeMult(treeState.pieceLength, treeConfig.pieceLengthFct or 1)

    local width = treePieces(entity, treeConfig, treeState)
    entityTable(entity, parent, treeState.rotation, mtlib.range(treeState.zIndex, treeState.zIndex))
    setUpdateFunction(entity, .1, updateFunction(width, length, treeState))

    treeState.length      = treeState.length + length
    treeState.totalLength = treeState.totalLength - length
    treeState.pieces      = treeState.pieces - 1
    treeState.totalPieces = treeState.totalPieces - 1
    mtlib.rangeDiv(treeState.pieceLength, treeConfig.pieceLengthFct or 1)

    return width
end

function continueBranch (parent, treeConfig, treeState)
    local angle        = mtlib.random(treeConfig.pieceAngle)
    local bRotation    = treeState.rotation
    treeState.rotation = mtlib.rotate(treeState.rotation, angle)

    local width        = piece(parent, treeConfig, treeState)

    treeState.rotation = bRotation
    return width
end

function branch(parent, treeConfig, treeState)
    if not mtlib.chance(treeConfig.branchChance) then return 0 end
    local angle = mtlib.random(treeConfig.branchAngle)
    local width = doBranch(parent, treeConfig, treeState, angle)
    return width
end

function crown(parent, treeConfig, treeState)
    local amount = mtlib.random(treeConfig.crownBranches)
    local angles = mtlib.randomDistrMin(treeConfig.crownAngle, treeConfig.crownMinAngle, amount)
    local width  = 0
    for _, angle in pairs(angles) do
        width = width + doBranch(parent, treeConfig, treeState, angle)
    end
    return width
end

function doBranch(parent, treeConfig, treeState, angle)
    local bRotation    = treeState.rotation
    local bLength      = treeState.length
    treeState.rotation = mtlib.rotate(treeState.rotation, angle)
    treeState.length   = 0
    treeState.depth    = treeState.depth + 1
    mtlib.rangeMult(treeState.pieceAmount, treeState.pieceAmountFct or 1)
    treeState.branchLength = treeState.branchLength * (treeConfig.branchLengthFct or 1)

    local width            = piece(parent, treeConfig, treeState)

    treeState.rotation     = bRotation
    treeState.length       = bLength
    treeState.depth        = treeState.depth - 1
    mtlib.rangeDiv(treeState.pieceAmount, treeState.pieceAmountFct or 1)
    treeState.branchLength = treeState.branchLength / (treeConfig.branchLengthFct or 1)
    return width
end

function treePieces     (parent, treeConfig, treeState)
    -- Check if branch piece limits have been reached.
    if mtlib.upperLimit(treeConfig.limitLength, treeState.totalLength) or
            mtlib.upperLimit(treeConfig.limitPieces, treeState.totalPieces) then
        return 0
    end

    local width = mtlib.random(treeConfig.widthIncrease)

    -- Check if this branch has finished.
    if not mtlib.upperLimit(treeState.pieceAmount, treeState.pieces) and
            not mtlib.upperLimit(treeState.branchLength, treeState.length) then
        -- Not finished, continue branch
        width = width + continueBranch(parent, treeConfig, treeState)
        if not mtlib.upperLimit(treeConfig.limitDepth, treeState.depth) then
            -- Create new branch from this point, if depth limit has not been reached
            width = width + branch(parent, treeConfig, treeState)
        end
    else if not mtlib.upperLimit(treeConfig.limitDepth, treeState.depth) then
        --Finished, create crown if depth limit has not been reached
        width = width + crown(parent, treeConfig, treeState)
    end
    end

    return width
end
