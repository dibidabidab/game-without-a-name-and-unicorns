
persistenceMode(TEMPLATE | ARGS | SPAWN_POS | REVIVE)

function create(key)
    setName(key, "key")
    component.AABB.getFor(key).halfSize = ivec2(5, 5)
    setComponents(key, {
        PointLight {
            radius = 60
        },
        ModelView {
            modelCollection = "models/key",
            modelName = "key",
            scale = vec3(.3),
            locationOffset = vec3(0, 0, -20)
        },
        PlayerDetector {
            distance = 16
        }
    })

    local animDir = 1

    local animFunction = nil
    animFunction = function()

        animDir = animDir * -1

        local rot = quat:new()
        rot.y = 40 * animDir
        rot.z = 10 * animDir

        component.ModelView.animate(key, "locationOffset", vec3(0, 10 * animDir, -20), 1., "pow2", animFunction)
        component.ModelView.animate(key, "rotation", rot, 1., "pow2")
    end
    animFunction()

    onEntityEvent(key, "PlayerDetected", function (player)

        setTimeout(key, 0., function()  -- todo: emitting events from c++ sucks.

            currentEngine.playerHasKey = true

            if hudScreen ~= nil then
                hudScreen.applyTemplate(hudScreen.createEntity(), "InventoryItem", {
                    icon = "sprites/ui/key_icon"
                })
            end

            component.PlayerDetector.remove(key)
            component.ModelView.animate(key, "scale", vec3(.6), .07, "pow2Out", function()
                component.ModelView.animate(key, "scale", vec3(.0), .1, "pow2In", function()

                    setTimeout(key, 0., function()  -- todo: calling lua from c++ sucks.
                        destroyEntity(key)
                    end)
                end)
            end)
        end)
    end)
end
