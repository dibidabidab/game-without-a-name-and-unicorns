
persistenceMode(ALL_COMPONENTS)

function create(light)
    setComponents(light, {
        AABB {
            halfSize = ivec2(3)
        },
        DirectionalLight()
    })
end
