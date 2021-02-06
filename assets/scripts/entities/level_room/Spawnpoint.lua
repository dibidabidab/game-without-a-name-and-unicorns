
function create(sp)
    if not setName(sp, "Spawnpoint") then
        error("There already is a Spawnpoint!")
    end
    component.AABB.getFor(sp).halfSize = ivec2(2)
end
