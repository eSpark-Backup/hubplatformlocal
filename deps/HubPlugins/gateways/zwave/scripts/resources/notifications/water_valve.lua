local function convert_switch_value(value)
    if value == "on" then
        return "valve_is_opened"
    elseif value == "off" then
        return "valve_is_closed"
    else
        error("Unsupported water valve value: " .. tostring(value))
    end
end

local water_valve_state_data = {
    get_enum_values = function (event_type) return { "valve_is_opened", "valve_is_closed" } end,
    get_value = function (event_type, notification_value) return convert_switch_value(notification_value) end,
}

local water_valve_short_circuit_data = {
    can_be_reset_by_idle_event = true,
    value_after_reset = "no_short_circuit",
    initial_enum = { "no_short_circuit" },
    get_enum_values = function (event_type) return { "short_circuit_detected" } end,
    get_value = function (event_type, notification_value) return "short_circuit_detected" end
}

local water_valve_current_alarm_data = {
    can_be_reset_by_idle_event = true,
    value_after_reset = "current_ok",
    initial_enum = { "current_ok" },
    get_enum_values = function (event_type)
        return { "current_ok", "current_below_low_threshold", "current_above_high_threshold", "current_max" }
    end,
    get_value = function (event_type, notification_value) return notification_value end
}

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").water_valve,

    items_data = {
        water_valve_state = water_valve_state_data,
        master_water_valve_state = water_valve_state_data,
        water_valve_short_circuit = water_valve_short_circuit_data,
        master_water_valve_short_circuit = water_valve_short_circuit_data,
        water_valve_current_alarm = water_valve_current_alarm_data,
        master_water_valve_current_alarm = water_valve_current_alarm_data,
    },

    events_to_item_names = {
        valve_operation = "water_valve_state",
        master_valve_operation = "master_water_valve_state",
        valve_short_circuit = "water_valve_short_circuit",
        master_valve_short_circuit = "master_water_valve_short_circuit",
        valve_current_alarm = "water_valve_current_alarm",
        master_valve_current_alarm = "master_water_valve_current_alarm",
    },
}
