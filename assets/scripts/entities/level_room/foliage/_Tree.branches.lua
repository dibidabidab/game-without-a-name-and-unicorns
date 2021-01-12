mtlib       = include("scripts/util/math")
fslib       = include("scripts/util/wrappert")
treeConfigs = include("scripts/entities/level_room/foliage/_Tree.config")

function entityTable(pieceEntity, parent, angle, zIndex)

    setComponents(pieceEntity, {
        AABB(),

        DrawPolyline {
            colors      = { colors.wood },
            lineWidth   = 0,
            zIndexBegin = zIndex.min,
            zIndexEnd   = zIndex.max,
        },

        VerletRope {
            length     = 0,
            gravity    = vec2(rotate2d(0, 100, angle)[1], rotate2d(0, 100, angle)[2]), -- todo

            friction   = .7,
            nrOfPoints = 1,
            --moveByWind = 1.5,
        },

        AttachToRope {
            ropeEntity = parent,
            x          = 1,
        }
    })
end

function updateFunction(width, length, treeState)
    return function(dTime, entity, _)
        local lengthRange    = mtlib.range(length)
        local widthRange     = mtlib.range(width)
        local growthStart    = mtlib.interpolate(mtlib.range(treeState.baseWidth), treeConfigs.ageRangeDs,
                                                 math.sqrt(treeState.baseWidth) - math.sqrt(width))
        local growthRange    = mtlib.range(growthStart, treeConfigs.ageRange.max)

        local updateFunction = function(dTime, entity, _)
            local currentLength = mtlib.interpolateCap(growthRange, lengthRange, treeState.age)
            local currentWidth  = mtlib.interpolateCap(growthRange, widthRange, treeState.age)

            component.VerletRope.getFor(entity).length = currentLength

            component.DrawPolyline.getFor(entity).lineWidth = math.sqrt(currentWidth)


            if treeState.age >= 100 then
                setUpdateFunction(entity, 0, nil)
                return
            end

            if treeState.rootEntity == entity then
                treeState.age = treeState.age + dTime * 10
            end
        end

        setUpdateFunction(entity, .1, updateFunction)
        updateFunction(dTime, entity, nil)
    end
end

function piece          (parent, treeConfig, treeState, entity)
    local entity       = entity or createChild(parent, "branch")
    local length = mtlib.random(treeState.pieceLength)

    mtlib.rangeMult(treeState.pieceLength, treeConfig.pieceLengthFct or 1)

    local width = fslib.wrappert(treeState, function(update)
        update("length", treeState.length + length)
        update("totalLength", treeState.totalLength + length)
        update("pieces", treeState.pieces + 1)
        update("totalPieces", treeState.totalPieces + 1)
        update("tendencyStrength", treeState.tendencyStrength * (treeConfig.tendencyStrengthRdc or 1))

        tendency(treeConfig, treeState, update)
        local width = treePieces(entity, treeConfig, treeState)
        entityTable(entity, parent, treeState.angle, mtlib.range(treeState.zIndex, treeState.zIndex))
        setUpdateFunction(entity, .1, updateFunction(width, length, treeState))


        component.VerletRope.getFor(entity).moveByWind = 15 / math.max(width, 1)

        if treeState.totalLength - width * treeConfig.limitLength * .06 > treeState.leavesStart * treeConfig.limitLength then

            local leave = createChild(entity)

            setComponents(leave, {
                AABB(),
                AttachToRope {
                    ropeEntity = entity,
                    x          = .5
                },
                AsepriteView {
                    sprite = "sprites/tree_leaves",
                    zIndex = treeConfig.zIndex,
                    frame  = 3,
                    flipHorizontal = treeState.angle > 0 and true or false,
                    flipVertical   = math.abs(treeState.angle) > 90 and true or false
                },
            })
        end

        return width
    end)

    mtlib.rangeDiv(treeState.pieceLength, treeConfig.pieceLengthFct or 1)

    return width
end

function tendency (treeConfig, treeState, update)
    if treeConfig.tendency ~= "out" and treeConfig.tendency ~= "direction" then return end

    local angle = treeState.angle

    while math.abs(angle - treeState.tendencyDirection) > 180 do
        if angle < treeState.tendencyDirection then
            angle = angle + 360
        else
            angle = angle - 360
        end
    end

    update("angle", mtlib.linearSelect(mtlib.range(angle, treeState.tendencyDirection), treeState.tendencyStrength))
end

function continueBranch (parent, treeConfig, treeState)
    local angle = mtlib.random(treeConfig.pieceAngle)

    return fslib.wrappert(treeState, function(update)
        update("angle", treeState.angle + angle)
        return piece(parent, treeConfig, treeState)
    end)
end

function branch(parent, treeConfig, treeState)
    if treeState.length < treeState.branchlessStart * treeConfig.branchLength then return 0 end
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

    mtlib.rangeMult(treeState.pieceAmount, treeState.pieceAmountFct or 1)

    local width = fslib.wrappert(treeState, function(update)
        update("angle", treeState.angle + angle)
        update("length", 0)
        update("depth", treeState.depth + 1)
        update("branchLength", treeState.branchLength * (treeConfig.branchLengthFct or 1))
        update("branchlessStart", mtlib.random(treeConfig.branchlessStart))
        update("leavesStart", mtlib.random(treeConfig.leavesStart))

        return piece(parent, treeConfig, treeState)
    end)

    mtlib.rangeDiv(treeState.pieceAmount, treeState.pieceAmountFct or 1)
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
    elseif not mtlib.upperLimit(treeConfig.limitDepth, treeState.depth) then
        --Finished, create crown if depth limit has not been reached
        width = width + crown(parent, treeConfig, treeState)
    end

    return width
end
