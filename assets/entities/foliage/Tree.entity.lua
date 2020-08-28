
function randomizeArgs()
    defaultArgs({
        zIndex = -600 - math.random(200),
        length = math.random(120, 220),
        appleChance = math.random() * .1
    })
end

randomizeArgs()

function create(tree, args)

    treeComponents = {
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
    setComponents(tree, treeComponents)

    addBranches = function(side)

        x = .1 + math.random() * .4

        repeat

            length = treeComponents.VerletRope.length * (.2 + math.random() * .4)

            xInv = 1. - x

            length = length * .3 + length * .7 * xInv

            addBranch(tree, treeComponents, side, x, length, true, args.zIndex - 16, args)

            x = x + .1 + math.random() * .1

        until x >= 1
    end

    addBranches(-1)
    addBranches(1)

    randomizeArgs()
end

addBranch = function(parentBranch, parentBranchComps, side, posAlongParent, length, addLeaves, zIndex, args) -- side should be -1 or 1, posAlongParent should be between 0 and 1

    local branch = createChild(parentBranch)

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

    local branchComponents = {
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
    setComponents(branch, branchComponents)

    -- BIG LEAVES:
    if addLeaves then

        local leavePosAlongBranch = math.min(18 / length, 1)

        repeat

            local leaves = createChild(branch)

            setComponents(leaves, {
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
            })

            local minStep = 5 / length
            local maxStep = minStep * 3

            leavePosAlongBranch = leavePosAlongBranch + minStep + math.random() * (maxStep - minStep)

        until leavePosAlongBranch > .9
    end
    -- END BIG LEAVES

    -- SMALL LEAVES:
    local leavePosAlongBranch = math.min(64 / length, 1)
    repeat

        local leave = createChild(branch)

        setComponents(leave, {
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
        })

        local minStep = 4 / length
        local maxStep = minStep * 2

        leavePosAlongBranch = leavePosAlongBranch + minStep + math.random() * (maxStep - minStep)

    until leavePosAlongBranch >= .96
    -- END SMALL LEAVES

    -- APPLE:
    if math.random() < args.appleChance then

        local apple = createChild(branch)

        applyTemplate(apple, "Apple")
        setComponents(apple, {
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
        })
    end
    -- END APPLE

    -- SUB BRANCHES:
    if length > 20 then

        local nrOfNewBranches = math.random(1, length > 32 and (length > 48 and 5 or 3) or 2)

        local _
        for _ = 1, nrOfNewBranches do

            local newLength = branchComponents.VerletRope.length * (.2 + math.random() * .3)
            local newPosAlongParent = math.random() * .5 + .4
            local newZIndex = zIndexBegin + newPosAlongParent * (zIndexEnd - zIndexBegin)

            addBranch(branch, branchComponents, math.random() > .5 and -1 or 1, newPosAlongParent, newLength, false, newZIndex, args)
        end
    end
    -- END SUB BRANCHES

end

