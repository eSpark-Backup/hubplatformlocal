local base = {
    get_enum_values = function (event_type) return { event_type } end,
    get_value = function (event_type, notification_value) return event_type end  
}

return {
    __index = function(t, k)
        if base[k] ~= nil then
            return base[k]
        elseif k == "initial_enum" and rawget(t, "value_after_reset") ~= nil then
            t.initial_enum = { rawget(t, "value_after_reset") }
            return t.initial_enum
        end
    end
}
