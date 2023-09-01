local self, values = ...

if values then
    local time_to_string = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/time_to_string")
    local time_from_string = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/time_from_string")

    local function normalize_fragment(value, max_value)
        return value and math.max( math.min(value, max_value), 0 ) or 0
    end

    local normalize_time = function(time)
        local hour, minute = time_from_string(time)
        return time_to_string( normalize_fragment( hour, 23 ), normalize_fragment( minute, 60 ), 0 )
    end

    for _, value in ipairs(values) do
        value.startTime = normalize_time(value.startTime)
        value.stopTime = normalize_time(value.stopTime)
    end
end

return values
