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
    year_day_slots = 5,
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

local weekly_user_code_intervals_item_id = storage:call("find_item_id", 
    "weekly_user_code_intervals", 
    "9000", 
    class_ids.schedule_entry_lock, 
    channel_0.id )

tests.assert_ne(weekly_user_code_intervals_item_id, nil, "check item id")

local external_setter = loadfile( "HUB:zwave/scripts/set_item_dictionary_value" )
local external_remover = loadfile( "HUB:zwave/scripts/remove_item_dictionary_value" )

local function send_daily_repeating_slot_change_event(slot, value)
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


local zwave_value_1 = {
    weekDays = { "monday", "thursday" },
    startHour = 11,
    startMinute = 50,
    durationHour = 1,
    durationMinute = 11
}

local zwave_value_2 = {
    weekDays = { "saturday", "sunday" },
    startHour = 22,
    startMinute = 55,
    durationHour = 2,
    durationMinute = 4
}

local weekly_item_value = {
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

external_setter({
        item_id = weekly_user_code_intervals_item_id,
        key = "5",
        value_type = "array.weekly_interval",
        value = weekly_item_value
    })

send_daily_repeating_slot_change_event(1, zwave_value_1)
send_daily_repeating_slot_change_event(2, zwave_value_2)


tests.assert_eq( core.get_item(weekly_user_code_intervals_item_id).value, { ["5"] = weekly_item_value } )


local run_migration = loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")

local default_changes = require("HUB:zwave/scripts/migration/version_changes/default")
default_changes.check_all_nodes = true
loadfile = loadfile("HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(require "storage", default_changes, 1, 2)

run_migration()


send_daily_repeating_slot_change_event(1, zwave_value_1)
send_daily_repeating_slot_change_event(2, zwave_value_2)

tests.assert_eq( core.get_item(weekly_user_code_intervals_item_id).value, { ["5"] = weekly_item_value } )
