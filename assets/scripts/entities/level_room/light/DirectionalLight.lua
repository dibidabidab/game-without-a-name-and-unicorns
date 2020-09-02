
persistenceMode(ALL_COMPONENTS)

function create(light)
    setComponents(light, {
        AABB = {
            halfSize = {3, 3}
        },
        DirectionalLight = {}
    })
end
