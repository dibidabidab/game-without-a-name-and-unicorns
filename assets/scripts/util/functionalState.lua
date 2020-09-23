function functionalState(state, fn)
    local ststate = {}

    function update (key, value)
        table.insert(ststate, { key, state[key] })
        state[key] = value
    end

    local ret = fn(update)

    for i = #ststate, 1, -1 do
        key, value = ststate[i]
        state[key] = value
    end

    return ret
end
