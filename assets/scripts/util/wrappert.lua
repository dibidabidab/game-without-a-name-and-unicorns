function wrappert(state, fn)
    local ststate = {}

    function update (key, value)
        table.insert(ststate, { key, state[key] })
        state[key] = value
    end

    local ret = fn(update)

    for i = #ststate, 1, -1 do
        kv           = ststate[i]
        state[kv[1]] = kv[2]
    end

    return ret
end
