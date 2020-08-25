
-- arg() zet waardes in de args table. Dus arg("hoi", 5) zorgt ervoor dat args.hoi == 5, TENZIJ het argument door een ander script is bepaald via applyTemplate().

arg("zIndex", -600 - math.random(200))      -- de zIndex van de boom, netzoals in CSS
arg("length", math.random(120, 220)) -- de lengte van de boomstam


-- BOOMSTAM:

print(entity) -- 'entity' is een integer om de boomstam te identificeren



components = { -- components is een collectie van components die behoren tot 'entity' (oftewel de boomstam).


    AABB = { -- dit is het vierkantje onderaan de boomstam die je met I of M kan 'vastgrijpen'
        halfSize = {3, 3}, -- vierkantje is 6x6 pixels
        center = {128, 64} -- de positie van de boom
    },

    AsepriteView = { -- dit zorgt ervoor dat er kleine grassprietjes worden getekent onderaan de boom
        sprite = "sprites/tree_leaves",
        frame = 4,
        zIndex = args.zIndex -- werkt hetzelfde als in CSS
    },

    VerletRope = {  -- elke tak/boomstam is eigenlijk een Rope, omdat we rope physics gebruiken.
        length = args.length,
        gravity = {0, 100}, -- zorgt ervoor dat de rope niet naar onder hangt, maar naar boven 'hangt'

        friction = .8, -- hoe stroef het touw beweegt. niet heel belangrijk
        nrOfPoints = 4, -- uit hoeveel punten bestaat het touw, ook niet echt belangrijk
        moveByWind = 6 -- boomstam beweegt heen en weer door de wind.
    },

    DrawPolyline = { -- om de VerletRope te tekenen.
        colors = {6}, -- gebruik de 7e kleur van het palet, kan je in de Palette editor zien.

        repeatX = 3, -- we willen dat de boomstam 4 pixels dik is, dus tekenen we de rope gwn 3 keer extra, elke keer 1 pixel horizontaal verder

        zIndexEnd = args.zIndex, zIndexBegin = args.zIndex
    },

}
-- EINDE BOOMSTAM



-- TAK:

branchName = "branch"
branch = createChild(branchName) -- hier maken we een nieuwe entity
-- omdat we de nieuwe entity met createChild() hebben gemaakt, wordt deze entity verwijderd als de boomstam wordt verwijderd.

print(branchName, branch) -- 'branch' is gwn een integer, netzoals 'entity' (de boomstam)

childComponents[branchName] = { -- dit zijn de components die worden toegevoegd aan de branch

    AABB = {}, -- zorgt voor een klein vierkantje aan het begin van de tak die je met I kan inspecteren

    DrawPolyline = {
        colors = {6},
        repeatX = 1, -- lees regel 38
        repeatY = 0, -- gebruik repeatY ipv repeatX als de tak horizontaal is.

        zIndexBegin = args.zIndex,
        zIndexEnd = args.zIndex + 10    -- de zIndex van de getekende lijn wordt geinterpolate tussen deze 2 waardes. (dit kan je gebruiken om de tak door een groot blad heen te laten steken)
    },


    VerletRope = {
        length = 100, -- maak de tak 100 pixels lang
        gravity = {500, 500}, -- zorgt ervoor dat de tak/rope naar rechtsboven gaat 'hangen'

        friction = .95,
        nrOfPoints = 1,
        moveByWind = 6
    },


    AttachToRope = {   -- we willen dat de tak vastzit aan de boomstam, en meebeweegt als de boomstam wiebelt ofzo

        ropeEntity = entity, -- geef aan aan welke rope deze tak vastgemaakt moet worden. 'entity' is de boomstam
        x = .8 -- deze tak komt te zitten op 80% van de hoogte van de boomstam
    }

}
-- EINDE TAK


-- GROOT BLAD:

leavesName = "leaves_big"
leaves = createChild(leavesName)

print(leavesName, leaves)

childComponents[leavesName] = {
    AsepriteView = {
        sprite = "sprites/tree_leaves",
        zIndex = args.zIndex + 5, -- dit is dezelfde zIndex als de zIndex van het midden van de tak, dus zal de tak daar door het grote blad heen steken

        frame = math.random(0, 2) -- 0 = klein blad, 1 = middelgroot, 2 = heel groot
    },

    AABB = {
        halfSize = {3, 3}
    },

    AttachToRope = {
        ropeEntity = branch, -- zie regel 53
        x = .5 -- plaats het grote blad op de helft van de tak
    },

    TemplateSpawner = { -- af en toe een klein vallend blaadje spawnen. Zie particles/TreeLeaveParticle.entity.lua
        templateName = "TreeLeaveParticle",
        minDelay = 4,
        maxDelay = 100,
        minQuantity = 1,
        maxQuantity = 1
    },
}
-- EINDE GROOT BLAD


-- KLEIN BLAD:
smallLeaveName = "small_leave"
smallLeave = createChild(smallLeaveName)

print(smallLeaveName, smallLeave)

childComponents[smallLeaveName] = {

    AABB = {},
    AttachToRope = {
        ropeEntity = branch,
        x = .8
    },
    AsepriteView = {
        sprite = "sprites/tree_leaves",
        zIndex = args.zIndex + 20,
        frame = 3,

        -- flip het kleine blaadje op basis van de richting van de tak:

        flipHorizontal = childComponents[branchName].VerletRope.gravity[1] < 0 and true or false,
        flipVertical = childComponents[branchName].VerletRope.gravity[2] < 0 and true or false
    }

}
-- EINDE KLEIN BLAD
