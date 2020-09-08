mtlib      = include("scripts/util/math")

maxAge     = 100
ageRange   = mtlib.range(maxAge)
ageRangeDs = mtlib.range(maxAge - 1)

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
    branchlessStart         factorRange Amount of branch from the start that cannot have any branches split off of it
    width                   factorRange Stem width factor. 1 is normal width. Width depends on size and weight
    widthIncrease           factor      Increase of width for each branch piece

    pieceLength             lengthRange Length of tree pieces
    pieceAmount             amountRange Amount of tree pieces in branch
    pieceAngle              degreeRange Angle of a branch continuation piece

    branchChance            factor      Chance of branches splitting off
    branchAngle             degreeRange Angle of branches splitting off
    branchLength            length      Max length of the stem at full age. Length at age 0 is 0

    crownBranches           amountRange Amount of branches coming out of the crown of a branch
    crownAngle              degreeRange Angle of branches at the crown
    crownMinAngle           degrees     Minimum angle between branches at the crown

    tendency                enum        "out", "direction", "none": tendency for the tree to grow towards
    tendencyDirection       degrees     Direction of the tendency when using "direction"
    tendencyOutRange        degrees     Degrees of tendency "out" at the bottom of the branchable part of the stem
    tendencyStrength        factor      Strength of tendency
    tendencyStrengthRdc     factor      Each piece should have a reduced tendency (0-.9) or not (1)

    branchLengthFct         factor      Each branch should have a reduced length (0-.9) or not (1)
    pieceAmountFct          factor      Each branch should have a reduced nr of pieces (0-.9) or not (1)
    pieceLengthFct          factor      Each branch piece should have a reduced length (0-.9) or not (1)

    limitLength             length      Limit the total length of recursive branches
    limitDepth              amount      Limit the depth of recursive branches
    limitPieces             amount      Limit the depth of recursive branch pieces
]]--

oak        = {
    branchlessStart     = mtlib.range(.2, .4),
    width               = mtlib.range(1, 1.4),
    widthIncrease       = mtlib.range(.4),

    pieceLength         = mtlib.range(4, 16),
    pieceAngle          = mtlib.sqRangeDev(40),

    branchChance        = .4,
    branchAngle         = mtlib.rangeDev(40),
    branchLength        = 50,

    crownBranches       = mtlib.range(1, 2),
    crownAngle          = mtlib.rangeDev(60),
    crownMinAngle       = 25,

    tendency            = "direction",
    tendencyDirection   = 0,
    tendencyOutRange    = 0,
    tendencyStrength    = .05,
    tendencyStrengthRdc = .99,

    pieceLengthFct      = .99,
    pieceAmountFct      = .8,
    branchLengthFct     = .8,

    limitLength         = 200
}
