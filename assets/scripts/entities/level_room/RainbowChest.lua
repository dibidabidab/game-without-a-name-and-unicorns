
persistenceMode(TEMPLATE | ARGS | FINAL_POS)

function create(chest)
    component.AABB.getFor(chest).halfSize = ivec2(5, 8)
    setComponents(chest, {
        Physics (),
        AsepriteView {
            sprite = "sprites/chest",
            loop = false
        },
        PointLight {
            radius = 60
        },
        PlayerDetector {
            distance = 24
        },
        Health {
            maxHealth = 1,
            currHealth = 1,
            takesDamageFrom = {"key"},
            givePlayerArrowOnKill = "RainbowArrow",
            bloodColor = 0
        }
    })
    local opened = false
    onEntityEvent(chest, "PlayerDetected", function (player)

        setTimeout(chest, 0., function()  -- todo: emitting events from c++ sucks.

            if opened or not valid(player) or not currentEngine.playerHasKey then
                return
            end

            opened = true

            playAsepriteTag(component.AsepriteView.getFor(chest), "open", true)

            component.Health.getFor(chest).attacks:add(Damage {
                type = "key",
                points = 1,
                dealtBy = player
            })

            setTimeout(chest, .5, function()

                local door = getByName("EndDoor")
                if door ~= nil then
                    playAsepriteTag(component.AsepriteView.getFor(door), "open", true)

                    component.PlayerDetector.getFor(getChild(door, "detector")).distance = 10
                end
            end)
        end)
    end)
end
