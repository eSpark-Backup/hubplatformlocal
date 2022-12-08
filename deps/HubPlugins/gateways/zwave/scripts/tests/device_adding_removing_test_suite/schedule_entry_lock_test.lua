local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local cc_user_code = {
    id = class_ids.user_codes,
    supported_users_count = 250
}

local cc_schedule_entry_lock= {
    id = class_ids.schedule_entry_lock,
    week_day_slots = 10,
    year_day_slots = 2,
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

function zwave:get_node_info( node_id )
    return node
end

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )


-- check adding result
local expected_items = {
    {
        name = "user_codes",
        cc_id = class_ids.user_codes,
        channel_id = 0
    },
    {
        name = "user_codes_scan_progress",
        cc_id = class_ids.user_codes,
        channel_id = 0,
        value_type = item_value_type.int,
        value = 0,
    },
    {
        name = "daily_user_code_intervals",
        cc_id = class_ids.schedule_entry_lock,
        channel_id = 0,
        value_type = "dictionary.array.daily_interval",
        value = {},
        elements_max_number_2 = 2,
        force_checking_fields = { "elements_max_number", "one_week_day_cost", "one_shifted_week_day_cost" }
    },
    {
        name = "weekly_user_code_intervals",
        cc_id = class_ids.schedule_entry_lock,
        channel_id = 0,
        value_type = "dictionary.array.weekly_interval",
        value = {},
        tag = "week_day_per_slot",
        elements_max_number_2 = 10,
        one_week_day_cost = 1,
        one_shifted_week_day_cost = 2,
        force_checking_fields = { "elements_max_number" }
    }
}

local expected_devices = {
    {
        name = "Unknown",
        category = "unknown",
        items = expected_items,
        node_id = "9000",
        battery_powered = true
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- send slot values report to create storage mappings
events_handler( {
        event = "week_day_entry_lock_schedule_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.schedule_entry_lock,
        user_id = 5,
        slot_id = 1,
        value = {
            weekDay = "monday",
            startHour = 11,
            startMinute = 20,
            stopHour = 11,
            stopMinute = 27,
        }
    } )

events_handler( {
        event = "year_day_entry_lock_schedule_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.schedule_entry_lock,
        user_id = 4,
        slot_id = 1,
        value = {
            startYear = 2020,
            startMonth = 3,
            startDay = 1,
            startHour = 10,
            startMinute = 20,
            stopYear = 2020,
            stopMonth = 3,
            stopDay = 1,
            stopHour = 10,
            stopMinute = 50,
        }
    } )

-- request removing slots from ui
local external_remover = loadfile( "HUB:zwave/scripts/remove_item_dictionary_value" )

zwave.set_week_day_entry_lock_schedule = tests.create_mock()
zwave.request_week_day_entry_lock_schedule = tests.create_mock()
zwave.set_year_day_entry_lock_schedule = tests.create_mock()
zwave.request_year_day_entry_lock_schedule = tests.create_mock()

external_remover({ 
    item_id = assert(storage:call("find_item_id", 
                                  "weekly_user_code_intervals", 
                                  "9000", 
                                  class_ids.schedule_entry_lock, 
                                  channel_0.id,
                                  "week_day_per_slot" )), 
    key = "5" 
})
external_remover({ 
    item_id = assert(storage:call("find_item_id", 
                                  "daily_user_code_intervals", 
                                  "9000", 
                                  class_ids.schedule_entry_lock, 
                                  channel_0.id )), 
    key = "4" 
})

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )

-- check removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )

tests.assert_eq(require("storage").empty(), true, "check storage empty (intervals mappings are removed)")
