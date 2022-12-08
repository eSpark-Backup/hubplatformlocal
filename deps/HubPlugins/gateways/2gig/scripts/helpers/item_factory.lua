local item_names = require("HUB:2gig/scripts/enum/item_names")

local item_factory = {}

function item_factory.create_button_state_item(button_number, states)
    local button_states = require("HUB:2gig/scripts/enum/button_states")

    return {
        name = item_names.button_state,
        tag = button_number,
        value_type = "button_state",
        enum = states,
        value = {
            button_number = button_number,
            button_state = button_states.idle
        },
        has_getter = true,
        has_setter = false
    }
end

function item_factory.create_door_window_state_item()
    local door_window_states = require("HUB:2gig/scripts/enum/door_window_states")

    return {
        name = item_names.door_window_state,
        value_type = "token",
        enum = {
            door_window_states.dw_is_opened,
            door_window_states.dw_is_closed,
            door_window_states.unknown
        },
        value = door_window_states.unknown,
        has_getter = true,
        has_setter = false
    }
end

function item_factory.create_battery_item()
    return {
        name = item_names.battery,
        value_type = "int",
        value = 100,
        has_getter = true,
        has_setter = false
    }
end

function item_factory.create_motion_item()
    return {
        name = item_names.motion,
        value_type = "bool",
        value = false,
        has_getter = true,
        has_setter = false
    }
end

function item_factory.create_security_threat_item()
    return {
        name = item_names.security_threat,
        value_type = "bool",
        value = false,
        has_getter = true,
        has_setter = false
    }
end

function item_factory.create_user_codes_item()
    return {
        name = item_names.user_codes,
        value_type = "dictionary.userCode",
        value = {},
        has_getter = true,
        has_setter = true
    }
end

function item_factory.create_user_code_action_item()
    local user_code_actions = require("HUB:2gig/scripts/enum/user_code_actions")

    return {
        name = item_names.user_code_action,
        value_type = "user_code_action",
        value = {
            action = user_code_actions.unknown,
            userId = -1
        },
        has_getter = true,
        has_setter = false
    }
end

function item_factory.create_week_user_code_restriction_item()
    return {
        name = item_names.weekly_user_code_intervals,
        value_type = "dictionary.array.weekly_interval",
        value = {},
        has_getter = true,
        has_setter = true
    }
end

function item_factory.create_day_user_code_restriction_item()
    return {
        name = item_names.daily_user_code_intervals,
        value_type = "dictionary.array.daily_interval",
        value = {},
        has_getter = true,
        has_setter = true
    }
end

local function create_alarm_item(item_name, alarm_states)
    return {
        name = item_name,
        value_type = "token",
        enum = {
            alarm_states.ok,
            alarm_states.not_ok,
            alarm_states.unknown
        },
        value = alarm_states.unknown,
        has_getter = true,
        has_setter = false
    }
end

function item_factory.create_tampering_cover_alarm_item()
    local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")

    return create_alarm_item(item_names.tampering_cover_alarm, tampering_cover_states)
end

function item_factory.create_tampering_move_alarm_item()
    local tampering_move_states = require("HUB:2gig/scripts/enum/tampering_move_states")

    return create_alarm_item(item_names.tampering_move_alarm, tampering_move_states)
end

function item_factory.create_smoke_alarm_item()
    local smoke_alarm_states = require("HUB:2gig/scripts/enum/smoke_alarm_states")

    return create_alarm_item(item_names.smoke_alarm, smoke_alarm_states)
end

function item_factory.create_heat_alarm_item()
    local heat_alarm_states = require("HUB:2gig/scripts/enum/heat_alarm_states")

    return create_alarm_item(item_names.heat_alarm, heat_alarm_states)
end

function item_factory.create_freeze_alarm_item()
    local freeze_alarm_states = require("HUB:2gig/scripts/enum/freeze_alarm_states")

    return create_alarm_item(item_names.freeze_alarm, freeze_alarm_states)
end

function item_factory.create_glass_breakage_alarm_item()
    local glass_breakage_states = require("HUB:2gig/scripts/enum/glass_breakage_states")

    return create_alarm_item(item_names.glass_breakage_alarm, glass_breakage_states)
end

function item_factory.create_co_alarm_item()
    local co_alarm_states = require("HUB:2gig/scripts/enum/co_alarm_states")

    return create_alarm_item(item_names.co_alarm, co_alarm_states)
end

function item_factory.create_water_leak_alarm_item()
    local water_leak_alarm_states = require("HUB:2gig/scripts/enum/water_leak_alarm_states")

    return create_alarm_item(item_names.water_leak_alarm, water_leak_alarm_states)
end

return item_factory
