local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local core = require("core")

zwave.request_node_values = tests.create_mock()

zwave.set_daily_repeating_entry_lock_schedule = tests.create_mock()
zwave.request_daily_repeating_entry_lock_schedule = tests.create_mock()

local cc_user_code = {
    id = class_ids.user_codes,
    supported_users_count = 250
}

local cc_schedule_entry_lock = {
    id = class_ids.schedule_entry_lock,
    year_day_slots = 0,
    week_day_slots = 0,
    daily_repeating_slots = 10
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
    channel_0.id )

tests.assert_ne(item_id, nil, "check item id")

local external_setter = loadfile( "HUB:zwave/scripts/set_item_dictionary_value" )
local external_remover = loadfile( "HUB:zwave/scripts/remove_item_dictionary_value" )

local function check_set_and_request_called(slot, zwave_value, description)
    tests.assert_called(
        zwave.set_daily_repeating_entry_lock_schedule, 
        "check set daily repeating entry lock schedule after " .. description, 
        { { "9000", 0, 5, slot, zwave_value } })

    tests.assert_called(
        zwave.request_daily_repeating_entry_lock_schedule, 
        "check daily repeating repeating entry lock schedule after " .. description, 
        { { "9000", 0, 5, slot } })
end

local function check_set_and_request_not_called(description)
    tests.assert_called(
        zwave.set_daily_repeating_entry_lock_schedule, 
        "check don't call set daily repeating entry lock schedule after " .. description, 
        { })

    tests.assert_called(
        zwave.request_daily_repeating_entry_lock_schedule, 
        "check don't request daily repeating entry lock schedule after " .. description, 
        { })
end

local zwave_value_1 = {
    weekDays = { "monday", "thursday" },
    startHour = 11,
    startMinute = 50,
    durationHour = 1,
    durationMinute = 11
}

local zwave_value_2 = {
    weekDays = { "sunday", "saturday" },
    startHour = 22,
    startMinute = 55,
    durationHour = 2,
    durationMinute = 4
}

local function send_slot_change_event(slot, value)
    events_handler( {
        event = "daily_repeating_entry_lock_schedule_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.schedule_entry_lock,
        user_id = 5,
        slot_id = slot,
        value = value
    } )
end

do -- set slot values from ui
    external_setter({
            item_id = item_id,
            key = "5",
            value_type = "array.weekly_interval",
            value = {
                {
                    weekDays = { "monday", "thursday" },
                    startTime = "11:50:00",
                    stopTime = "13:01:00",
                },
                {
                    weekDays = { "saturday", "sunday" },
                    startTime = "22:55:00",
                    stopTime = "00:59:00",
                }
            }
        })

    check_set_and_request_called(1, zwave_value_1, "external value set")

    tests.assert_eq(core.get_item(item_id).value, {}, "check empty dictionary after external value set")
end

do -- report first slot from zwave
    send_slot_change_event(1, zwave_value_1)

    tests.assert_eq(
        core.get_item(item_id).value, 
        {
            ["5"] = {
                {
                    weekDays = { "monday", "thursday" },
                    startTime = "11:50:00",
                    stopTime = "13:01:00"
                }
            }
        }, 
        "check value after first report from zwave")

    check_set_and_request_called(2, zwave_value_2, "first slot changing")
end

do -- unexpected report first slot from zwave
    zwave_value_1.startMinute = zwave_value_1.startMinute + 1
    table.insert(zwave_value_1.weekDays, "wednesday" )
    send_slot_change_event(1, zwave_value_1)

    tests.assert_eq(
        core.get_item(item_id).value, 
        {
            ["5"] = {
                {
                    weekDays = { "monday", "wednesday", "thursday" },
                    startTime = "11:51:00",
                    stopTime = "13:02:00"
                }
            }
        }, 
        "check value after unexpected 1st slot report from zwave")

    check_set_and_request_not_called("unexpected 1st slot report from zwave")
end

do -- report 2nd slot from zwave
    send_slot_change_event(2, zwave_value_2)

    tests.assert_eq(
        core.get_item(item_id).value, 
        {
            ["5"] = {
                {
                    weekDays = { "monday", "wednesday", "thursday" },
                    startTime = "11:51:00",
                    stopTime = "13:02:00"
                },
                {
                    weekDays = { "sunday", "saturday" },
                    startTime = "22:55:00",
                    stopTime = "00:59:00",
                }
            }
        }, 
        "check value after 2nd slot report from zwave")

    check_set_and_request_not_called("last expected slot report from zwave")
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
    check_set_and_request_not_called("all slots removing")

    tests.assert_eq(core.get_item(item_id).value, {}, "check dictionary is empty after all removing reports")
end