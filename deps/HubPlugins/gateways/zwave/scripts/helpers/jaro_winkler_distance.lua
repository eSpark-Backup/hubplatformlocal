-- Visit Wikipedia page about Jaro-Winkler distance algorithm:
-- https://en.wikipedia.org/wiki/Jaro%E2%80%93Winkler_distance

local a, b = ...

local jaro_winkler_prefix_size = 4
local jaro_winkler_scaling_factor = 0.1
local jaro_winkler_boost_threshold = 0.7

local distance = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/jaro_distance")(a, b)

if distance > jaro_winkler_boost_threshold then
    local common_prefix = 0
    local last_index = math.min(math.min(a:len(), b:len()), jaro_winkler_prefix_size)
    for idx = 1, last_index do
        if a:sub(idx, idx) == b:sub(idx, idx) then
            common_prefix = common_prefix + 1
        else
            break
        end
    end

    distance = distance + (jaro_winkler_scaling_factor * common_prefix * (1 - distance))
end

return distance
