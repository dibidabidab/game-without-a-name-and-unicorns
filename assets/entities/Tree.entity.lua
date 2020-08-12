
arg("length", math.random(120, 200))
arg("zIndex", -600 - math.random(200))

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
        gravity = {0, 250},
        friction = .8,
        nrOfPoints = 4
    },
    AsepriteView = {
        sprite = "sprites/tree_leaves",
        frame = 3,
        zIndex = args.zIndex
    }
}

branchNr = 0
leavesNr = 0

addBranch = function(parentBranch, parentBranchComps, side, posAlongParent, length, addLeaves, zIndex) -- side should be -1 or 1, posAlongParent should be between 0 and 1

    branchNr = branchNr + 1

    local branchName = "branch." .. branchNr
    print(branchName)

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

    local thickness = length > 52 and 3 or (length > 24 and 2 or 1)
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
            friction = .8,
            nrOfPoints = 1
        },
        AttachToRope = {
            ropeEntity = parentBranch,
            x = posAlongParent
        }
    }

    if addLeaves then

        leavePosAlongBranch = math.min(18 / length, 1)

        repeat

            leavesNr = leavesNr + 1

            leavesName = "leaves." .. leavesNr
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
                    zIndex = leavePosAlongBranch > .6 and zIndexBegin or zIndexEnd,--zIndexBegin + leavePosAlongBranch * (zIndexEnd - zIndexBegin),
                    frame = math.random(0, 2)
                }
            }

            minStep = 5 / length
            maxStep = minStep * 3

            leavePosAlongBranch = leavePosAlongBranch + minStep + math.random() * (maxStep - minStep)

        until leavePosAlongBranch > .9

    end

    if length > 20 then

        local nrOfNewBranches = math.random(1, length > 32 and (length > 48 and 5 or 3) or 2)

        local _
        for _ = 1, nrOfNewBranches do

            local newLength = childComponents[branchName].VerletRope.length * (.2 + math.random() * .3)
            local newPosAlongParent = math.random() * .6 + .3
            local newZIndex = zIndexBegin + newPosAlongParent * (zIndexEnd - zIndexBegin)

            addBranch(branch, childComponents[branchName], math.random() > .5 and -1 or 1, newPosAlongParent, newLength, false, newZIndex)
        end
    end

end

addBranches = function(side)

    x = .1 + math.random() * .3

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
