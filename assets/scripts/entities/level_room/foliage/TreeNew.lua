arg("ageArg", math.random(10, 100))
arg("zIndexArg", -600 - math.random(200))
arg("typeArg", "oak")

--[[ types in tree config
    length          Length in pixels
    factor          Factor of other measuree
    amount          An integer amount
    degrees         rotation
    enum            See description
    xRange          [min, max], with x length, factor, amount, degrees
]]--

--[[ TreeConfig
    length                  length      200     Max length of the stem at full age. Length at age 0 is 0
    branchlessStart         factorRange .3      Amount of branch from the start that cannot have any branches split off of it
    width                   factorRange 1       Stem width factor. 1 is normal width. Width depends on size and weight

    pieceLength             lengthRange 10      Length of tree pieces
    pieceAmount             amountRange         Amount of tree pieces

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
    limitBranches           amount              Limit the depth of recursive branches
    limitPieces             amount              Limit the depth of recursive branch pieces
]]--

treeConfigs = {
    default = {
    },
}

age = ageArg
zIndex = zIndexArg
type = typeArg
rootConfig = treeConfigs[type]

lib = {
    random = function(range)
        return math.random(range[1], range[2])
    end,
    cap = function(range, value)
        if value < range[1] then
            return range[1]
        end
        if value > range[2] then
            return range[2]
        end
        return value
    end,
    interpolate = function(from, to, value)
        value = value - from[1] / (from[2] - from[1])
        return value * (to[2] - to[1]) + to[1]
    end,
    interpolateCap = function(from, to, value)
        return lib.interpolate(from, to, lib.cap(from, value))
    end
}

-- Component functions

rootState = {
    rotation = { 0, 100 },
    zIndex = zIndex,
    zDelta = 0,
    length = 0,
    depth = 0,

    pieceLength = rootConfig.pieceLength,
    pieceAmount = rootConfig.pieceAmount,
    branchLength = rootConfig.branchLength
}

branch = function(parent, treeConfig, stateTable, entity)
    entity = entity or createChild(parent, "branch")

    length = treeConfig

    width = branches(entity, treeConfig, stateTable)

end

branches = function(treeConfig, age, parent, length, depth)


end

--[[

components = {
    AABB = {
        halfSize = {5, 5}
    },
    DrawPolyline = {
        colors = {6},
        repeatX = 3,
        zIndexEnd = args.zIndex, zIndexBegin = args.zIndex
    },
    VerletRope = {
        length = args.length,
        gravity = {0, 100},
        friction = .8,
        nrOfPoints = 4,
        moveByWind = 6
    },
    AsepriteView = {
        sprite = "sprites/tree_leaves",
        frame = 4,
        zIndex = args.zIndex
    }
}

branchNr = 0
leavesNr = 0

addBranch = function(parentBranch, parentBranchComps, side, posAlongParent, length, addLeaves, zIndex) -- side should be -1 or 1, posAlongParent should be between 0 and 1

    branchNr = branchNr + 1

    local branchName = "branch." .. branchNr
    local branch = createChild(branchName)

    ::begin::

    local rotateDegrees = 45
    if math.random() > .5 then
        rotateDegrees = 90
    end
    --rotateDegrees = rotateDegrees + math.random(-20, 0)
    rotateDegrees = rotateDegrees * side

    local gravity = rotate2d(parentBranchComps.VerletRope.gravity[1], parentBranchComps.VerletRope.gravity[2], rotateDegrees)

    local horizontal = math.abs(gravity[2]) < .1

    if horizontal and length > 20 then
        goto begin
    end

    local thickness = length > 64 and 3 or (length > 30 and 2 or 1)
    local xThickness = 1
    local yThickness = 1
    if horizontal then
        yThickness = thickness
    else
        xThickness = thickness
    end

    local zIndexBegin = zIndex
    local zIndexEnd = zIndex + 32

    childComponents[branchName] = {
        AABB = {},
        DrawPolyline = {
            colors = {6},
            repeatX = xThickness - 1,
            repeatY = yThickness - 1,
            zIndexBegin = zIndexBegin, zIndexEnd = zIndexEnd
        },
        VerletRope = {
            length = length,
            gravity = gravity,
            friction = .95,
            nrOfPoints = 1,
            moveByWind = 6
        },
        AttachToRope = {
            ropeEntity = parentBranch,
            x = posAlongParent
        }
    }

    -- BIG LEAVES:
    if addLeaves then

        local leavePosAlongBranch = math.min(18 / length, 1)

        repeat

            leavesNr = leavesNr + 1

            local leavesName = "leaves." .. leavesNr
            createChild(leavesName)

            childComponents[leavesName] = {
                AABB = {
                    halfSize = {3, 3}
                },
                AttachToRope = {
                    ropeEntity = branch,
                    x = leavePosAlongBranch
                },
                AsepriteView = {
                    sprite = "sprites/tree_leaves",
                    zIndex = leavePosAlongBranch > .7 and zIndexBegin or zIndexEnd,
                    frame = math.random(0, 2)
                },
                TemplateSpawner = {
                    templateName = "TreeLeaveParticle",
                    minDelay = 4,
                    maxDelay = 100,
                    minQuantity = 1,
                    maxQuantity = 1
                },
            }

            local minStep = 5 / length
            local maxStep = minStep * 3

            leavePosAlongBranch = leavePosAlongBranch + minStep + math.random() * (maxStep - minStep)

        until leavePosAlongBranch > .9
    end
    -- END BIG LEAVES

    -- SMALL LEAVES:
    local leavePosAlongBranch = math.min(64 / length, 1)
    repeat

        leavesNr = leavesNr + 1

        local leavesName = "leave_small." .. leavesNr
        createChild(leavesName)

        childComponents[leavesName] = {
            AABB = {},
            AttachToRope = {
                ropeEntity = branch,
                x = leavePosAlongBranch
            },
            AsepriteView = {
                sprite = "sprites/tree_leaves",
                zIndex = zIndexBegin + leavePosAlongBranch * (zIndexEnd - zIndexBegin),
                frame = 3,
                flipHorizontal = gravity[1] < 0 and true or false,
                flipVertical = gravity[2] < 0 and true or false
            }
        }

        local minStep = 4 / length
        local maxStep = minStep * 2

        leavePosAlongBranch = leavePosAlongBranch + minStep + math.random() * (maxStep - minStep)

    until leavePosAlongBranch >= .96
    -- END SMALL LEAVES

    -- APPLE:
    if math.random() < args.appleChance then
        local appleName = "apple." .. branchNr
        applyTemplate(createChild(appleName), "Apple")
        childComponents[appleName] = {
            AttachToRope = {
                ropeEntity = branch,
                x = math.random(),
                offset = {0, -4}
            },
            Physics = {
                gravity = 0
            },
            Health = {
                componentsToRemoveOnDeath = { "Physics" }
            },
            AsepriteView = {
                zIndex = zIndexEnd + 32
            }
        }
    end
    -- END APPLE

    -- SUB BRANCHES:
    if length > 20 then

        local nrOfNewBranches = math.random(1, length > 32 and (length > 48 and 5 or 3) or 2)

        local _
        for _ = 1, nrOfNewBranches do

            local newLength = childComponents[branchName].VerletRope.length * (.2 + math.random() * .3)
            local newPosAlongParent = math.random() * .5 + .4
            local newZIndex = zIndexBegin + newPosAlongParent * (zIndexEnd - zIndexBegin)

            addBranch(branch, childComponents[branchName], math.random() > .5 and -1 or 1, newPosAlongParent, newLength, false, newZIndex)
        end
    end
    -- END SUB BRANCHES

end

addBranches = function(side)

    x = .1 + math.random() * .4

    repeat

        length = components.VerletRope.length * (.2 + math.random() * .4)

        xInv = 1. - x

        length = length * .3 + length * .7 * xInv

        addBranch(entity, components, side, x, length, true, args.zIndex - 16)

        x = x + .1 + math.random() * .1

    until x >= 1
end

addBranches(-1)
addBranches(1)
]]--
