local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local tests = require("tests")

local function check_item(actual, expected)
    local keys = { "name", "value_type", "enum", "value" }

    for _, key in ipairs(keys) do
        tests.assert_eq(actual[key], expected[key], key)
    end
end

check_item(item_factory.create_button_state_item(123, { "foo", "bar" }), {
    name = "button_state",
    value_type = "button_state",
    enum = {
        "foo",
        "bar"
    },
    value = {
        button_number = 123,
        button_state = "idle"
    }
})

check_item(item_factory.create_door_window_state_item(), {
    name = "dw_state",
    value_type = "token",
    enum = {
        "dw_is_opened",
        "dw_is_closed",
        "unknown"
    },
    value = "unknown"
})

check_item(item_factory.create_battery_item(), {
    name = "battery",
    value_type = "int",
    value = 100
})

check_item(item_factory.create_motion_item(), {
    name = "motion",
    value_type = "bool",
    value = false
})

check_item(item_factory.create_security_threat_item(), {
    name = "security_threat",
    value_type = "bool",
    value = false
})

check_item(item_factory.create_user_codes_item(), {
    name = "user_codes",
    value_type = "dictionary.userCode",
    value = {}
})

check_item(item_factory.create_user_code_action_item(), {
    name = "user_code_action",
    value_type = "user_code_action",
    value = {
        action = "unknown",
        userId = -1
    }
})

check_item(item_factory.create_week_user_code_restriction_item(), {
    name = "weekly_user_code_intervals",
    value_type = "dictionary.array.weekly_interval",
    value = {}
})

check_item(item_factory.create_day_user_code_restriction_item(), {
    name = "daily_user_code_intervals",
    value_type = "dictionary.array.daily_interval",
    value = {}
})

check_item(item_factory.create_tampering_cover_alarm_item(), {
    name = "tampering_cover_alarm",
    value_type = "token",
    enum = {
        "no_tampering_cover",
        "tampering_cover",
        "unknown"
    },
    value = "unknown"
})

check_item(item_factory.create_tampering_move_alarm_item(), {
    name = "tampering_move_alarm",
    value_type = "token",
    enum = {
        "no_moving",
        "product_moved",
        "unknown"
    },
    value = "unknown"
})

check_item(item_factory.create_smoke_alarm_item(), {
    name = "smoke_alarm",
    value_type = "token",
    enum = {
        "no_smoke",
        "smoke_detected",
        "unknown"
    },
    value = "unknown"
})

check_item(item_factory.create_heat_alarm_item(), {
    name = "heat_alarm",
    value_type = "token",
    enum = {
        "heat_ok",
        "overheat_detected",
        "unknown"
    },
    value = "unknown"
})

check_item(item_factory.create_freeze_alarm_item(), {
    name = "freeze_alarm",
    value_type = "token",
    enum = {
        "no_freeze",
        "freeze_detected",
        "unknown"
    },
    value = "unknown"
})

check_item(item_factory.create_glass_breakage_alarm_item(), {
    name = "glass_breakage_alarm",
    value_type = "token",
    enum = {
        "no_glass_breakage",
        "glass_breakage",
        "unknown"
    },
    value = "unknown"
})

check_item(item_factory.create_co_alarm_item(), {
    name = "co_alarm",
    value_type = "token",
    enum = {
        "no_co",
        "co_detected",
        "unknown"
    },
    value = "unknown"
})

check_item(item_factory.create_water_leak_alarm_item(), {
    name = "water_leak_alarm",
    value_type = "token",
    enum = {
        "no_water_leak",
        "water_leak_detected",
        "unknown"
    },
    value = "unknown"
})
