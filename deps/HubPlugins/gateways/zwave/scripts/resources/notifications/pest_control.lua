local function convert_event_to_value(event)
    local data = {
        pest_removed = "pest_removed",
        trap_armed = "trap_armed",
        trap_rearm_required = "trap_rearm_required",
        pest_exterminated = "pest_exterminated",
    }

    local value = assert(data[event], "Unsupported pest_control alarm event: " .. tostring(event))
    return value
end
return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").pest_control,

    items_data = {
        pest_control = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "trap_rearm_required",
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
    },

    events_to_item_names = {
        pest_removed = "pest_control",
        trap_armed = "pest_control",
        trap_rearm_required = "pest_control",
        pest_exterminated = "pest_control",
    },
}
