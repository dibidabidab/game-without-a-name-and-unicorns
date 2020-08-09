
arg("length", math.random(120, 240))

components = {
    AABB = {
        halfSize = {5, 5}
    },
    DrawPolyline = {
        colors = {6},
        repeatX = 3,
        zIndexEnd = -64, zIndexBegin = -64
    },
    VerletRope = {
        length = args.length,
        gravity = {0, 250},
        friction = .8,
        nrOfPoints = 4
    },
}

addBranch = function(parentBranch, parentBranchComps, side, posAlongParent, length) -- side should be -1 or 1, posAlongParent should be between 0 and 1

    branchName = "branch." .. math.random(100000, 999999);

    branch = createChild(branchName)

    ::begin::

    rotateDegrees = 45
    if math.random() > .5 then
        rotateDegrees = 90
    end
    rotateDegrees = rotateDegrees * side

    gravity = rotate2d(parentBranchComps.VerletRope.gravity[1], parentBranchComps.VerletRope.gravity[2], rotateDegrees)

    horizontal = math.abs(gravity[2]) < .1

    if horizontal and length > 20 then
        goto begin
    end

    thickness = length > 52 and 3 or (length > 24 and 2 or 1)
    xThickness = 1
    yThickness = 1
    if horizontal then
        yThickness = thickness
    else
        xThickness = thickness
    end

    childComponents[branchName] = {
        AABB = {},
        DrawPolyline = {
            colors = {6},
            repeatX = xThickness - 1,
            repeatY = yThickness - 1,
            zIndexEnd = -64, zIndexBegin = -64
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
    if length > 20 then

        lengthNew = childComponents[branchName].VerletRope.length * (.2 + math.random() * .4)

        addBranch(branch, childComponents[branchName], math.random() > .5 and -1 or 1, math.random() * .9, lengthNew)
    end
end

addBranches = function(side)

    x = .1 + math.random() * .1

    repeat

        length = components.VerletRope.length * (.2 + math.random() * .4)

        xInv = 1. - x

        length = length * .3 + length * .7 * xInv

        addBranch(entity, components, side, x, length)

        x = x + .1 + math.random() * .1

    until x >= 1
end

addBranches(-1)
addBranches(1)
