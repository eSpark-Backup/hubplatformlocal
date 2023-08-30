local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local core = require("core")

zwave.request_node_values = tests.create_mock()

zwave.set_week_day_entry_lock_schedule = tests.create_mock()
zwave.request_week_day_entry_lock_schedule = tests.create_mock()

local cc_user_code = {
    id = class_ids.user_codes,
    supported_users_count = 250
}

local cc_schedule_entry_lock= {
    id = class_ids.schedule_entry_lock,
    week_day_slots = 10,
    year_day_slots = 0,
    daily_repeating_slots = 0
}

local channel_0 = {
    id = 0,
    classes = { cc_user_code, cc_schedule_entry_lock }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )


local item_id = storage:call("find_item_id",
    "weekly_user_code_intervals",
    "9000",
    class_ids.schedule_entry_lock,
    channel_0.id,
    "week_day_per_slot" )


tests.assert_ne(item_id, nil, "check item id")

local external_setter = loadfile( "HUB:zwave/scripts/set_item_dictionary_value" )
local external_remover = loadfile( "HUB:zwave/scripts/remove_item_dictionary_value" )

local zwave_values = {
    {
        weekDay = "monday",
        startHour = 8,
        startMinute = 50,
        stopHour = 13,
        stopMinute = 1
    },
    {
        weekDay = "sunday",
        startHour = 21,
        startMinute = 55,
        stopHour = 22,
        stopMinute = 59
    },
    {
        weekDay = "saturday",
        startHour = 21,
        startMinute = 55,
        stopHour = 22,
        stopMinute = 59
    }
}

local function send_slot_change_event(slot, value)
    events_handler( {
        event = "week_day_entry_lock_schedule_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.schedule_entry_lock,
        user_id = 5,
        slot_id = slot,
        value = value
    } )
end

local function check_set_and_request_called(slot, zwave_value, description)
    tests.assert_called(
        zwave.set_week_day_entry_lock_schedule,
        "check set week day entry lock schedule after " .. description,
        { { "9000", 0, 5, slot, zwave_value } })

    tests.assert_called(
        zwave.request_week_day_entry_lock_schedule,
        "check request week day entry lock schedule after " .. description,
        { { "9000", 0, 5, slot } })
end

local function check_set_and_request_not_called(description)
    tests.assert_called(
        zwave.set_week_day_entry_lock_schedule,
        "check don't call set week day entry lock schedule after " .. description,
        { })

    tests.assert_called(
        zwave.request_week_day_entry_lock_schedule,
        "check don't request week day entry lock schedule after " .. description,
        { })
end

do -- set slot values from ui
    external_setter({
            item_id = item_id,
            key = "5",
            value_type = "array.weekly_interval",
            value = {
                {
                    weekDays = { "monday" },
                    startTime = "8:50:00",
                    stopTime = "13:01:00",
                },
                {
                    weekDays = { "saturday", "sunday" },
                    startTime = "21:55:00",
                    stopTime = "22:59:00",
                }
            }
        })

    check_set_and_request_called(1, zwave_values[1], "external setting")
    tests.assert_eq(core.get_item(item_id).value, {}, "check empty dictionary after external value set")
end

do -- first slot report
    send_slot_change_event(1, zwave_values[1])
    check_set_and_request_called(2, zwave_values[2], "first slot report")

    tests.assert_eq(
        core.get_item(item_id).value,
        {
            ["5"] = {
                {
                    weekDays = { "monday" },
                    startTime = "08:50:00",
                    stopTime = "13:01:00"
                }
            }
        },
        "check weekly interval after first slot report")
end

do -- first slot report after item value reseted
    core.update_item_value(item_id, {})
    send_slot_change_event(1, zwave_values[1])

    tests.assert_eq(
        core.get_item(item_id).value,
        {
            ["5"] = {
                {
                    weekDays = { "monday" },
                    startTime = "08:50:00",
                    stopTime = "13:01:00"
                }
            }
        },
        "check weekly interval after first slot report")
end

do -- second slot report
    send_slot_change_event(2, zwave_values[2])
    check_set_and_request_called(3, zwave_values[3], "second slot report")

    tests.assert_eq(
        core.get_item(item_id).value,
        {
            ["5"] = {
                {
                    weekDays = { "monday" },
                    startTime = "08:50:00",
                    stopTime = "13:01:00"
                },
                {
                    weekDays = { "sunday" },
                    startTime = "21:55:00",
                    stopTime = "22:59:00"
                }
            }
        },
        "check weekly interval after second slot report")
end

do -- third slot report
    send_slot_change_event(3, zwave_values[3])
    check_set_and_request_not_called("last slot setting report")

    tests.assert_eq(
        core.get_item(item_id).value,
        {
            ["5"] = {
                {
                    weekDays = { "monday" },
                    startTime = "08:50:00",
                    stopTime = "13:01:00"
                },
                {
                    weekDays = { "sunday", "saturday" },
                    startTime = "21:55:00",
                    stopTime = "22:59:00"
                }
            }
        },
        "check completed weekly interval item value")
end

do -- external remove slots
    external_remover({
            item_id = item_id,
            key = "5"
        })
    check_set_and_request_called(1, nil, "external removing")

    send_slot_change_event(1, nil)
    check_set_and_request_called(2, nil, "first slot removing report")

    send_slot_change_event(2, nil)
    check_set_and_request_called(3, nil, "second slot removing report")

    tests.assert_eq(
        core.get_item(item_id).value,
        {
            ["5"] = {
                {
                    weekDays = { "saturday" },
                    startTime = "21:55:00",
                    stopTime = "22:59:00",
                }
            }
        },
        "check item value with setted third slot only")

    send_slot_change_event(3, nil)
    check_set_and_request_not_called("last slot removing report")
    tests.assert_eq(
        core.get_item(item_id).value,
        { },
        "check item value empty after all slots removing")
end
