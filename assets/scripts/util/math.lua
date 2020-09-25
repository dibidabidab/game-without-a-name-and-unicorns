function ntRange(min, max)
    return {
        min = min,
        max = max,
        fn  = function(value)
            if math.random(0, 1) == 0 then
                return value else
                return -value end
        end
    }
end

function rangeDev(dev)
    return range(-dev, dev)
end

function sqRangeDev(dev)
    return sqRange(-dev, dev)
end

function range(min, max)
    if (max == nil) then
        max = min
        min = 0
    end
    return {
        min = min,
        max = max,
        fn  = id
    }
end

function seed(seed)
    math.randomseed(seed)
    math.random()
    math.random()
    math.random()
end

function sqRange(min, max)
    if (max == nil) then
        max = min
        min = 0
    end
    return {
        min = psSqrt(min),
        max = psSqrt(max),
        fn  = psSquare,
    }
end

function rangeSize(range)
    return range.max - range.min
end

function rangeMult(range, value)
    if range == nil then return end
    range.min = range.min * value
    range.max = range.max * value
end

function rangeDiv (range, value)
    if range == nil then return end
    range.min = range.min / value
    range.max = range.max / value
end

resolution = 1000000
function random         (range)
    return range.fn(math.random(math.floor(range.min * resolution), math.floor(range.max * resolution)) / resolution)
end

function cap            (range, value)
    if range.max < range.min then
        return cap({ min = range.max, max = range.min, fn = range.fn })
    end
    if value < range.min then
        return range.min
    end
    if value > range.max then
        return range.max
    end
    return value
end

function interpolate    (from, to, value)
    value = (value - from.min) / (from.max - from.min)
    return value * (to.max - to.min) + to.min
end

function interpolateCap (from, to, value)
    return interpolate(from, to, cap(from, value))
end

function linearSelect(range, value)
    return range.fn(range.min * (1 - value) + range.max * value)
end

function rotate         (rotation, angle)
    return rotate2d(rotation[1], rotation[2], angle)
end

function upperLimit     (threshold, value)
    if threshold == nil or value == nil then return false end
    return value >= threshold
end

function lowerLimit     (value, threshold)
    if threshold == nil or value == nil then return false end
    return value <= threshold
end

function psSquare       (value)
    return value * value * sign(value)
end

function psSqrt         (value)
    return sign(value) * math.sqrt(value * sign(value))
end

function sign           (value)
    return select(value < 0, -1, 1)
end

function select         (selector, tt, ff)
    if selector then return tt else return ff end
end

function id             (v)
    return v
end

function chance         (chance)
    return math.random(0, resolution) / resolution < chance
end

function randomDistrMin(range, minDist, amount)
    local values = {}
    for i = 1, amount do
        for _ = 1, 10 do
            local angle    = random(range)
            local posSible = true
            for _, v in pairs(values) do
                if math.abs(v - angle) < minDist then posSible = false end
            end
            if posSible then
                values[i] = angle
                break
            end
        end
    end
    return values
end
