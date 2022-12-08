local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local cc_notification = {
    id = class_ids.notification,
    notifications = {
        {
            type = "appliance",
            event_types = { 
                "program_started",
                "program_in_progress",
                "program_completed",
                "supplying_water",
                "boiling",
                "washing",
                "rinsing",
                "draining",
                "spinning",
                "drying",
                "failure_to_set_target_temperature",
                "water_supply_failure",
                "boiling_failure",
                "washing_failure",
                "rinsing_failure",
                "draining_failure",
                "spinning_failure",
                "drying_failure",
                "fan_failure",
                "compressor_failure",
            }
        },
    }
}

local channel_0 = { 
    id = 0,
    classes = { cc_notification }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 0,
    product_id = 0,
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

local program_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/program_status")
local appliance_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/appliance_status")
local program_failed_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/program_failed_status")
local program_name_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/program_name")

-- check adding result
local expected_items = {
    {
        name = "program_status",
        tag = item_descriptor_tags.appliance,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            program_status_enum.unknown,
            program_status_enum.idle,
            program_status_enum.started,
            program_status_enum.in_progress,
            program_status_enum.completed
        },
        value = program_status_enum.unknown
    },
        {
        name = "appliance_status",
        tag = item_descriptor_tags.appliance,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            appliance_status_enum.unknown,
            appliance_status_enum.idle,
            appliance_status_enum.supplying_water,
            appliance_status_enum.boiling,
            appliance_status_enum.washing,
            appliance_status_enum.rinsing,
            appliance_status_enum.draining,
            appliance_status_enum.spinning,
            appliance_status_enum.drying,
        },
        value = appliance_status_enum.unknown
    },
    {
        name = "program_failures",
        tag = item_descriptor_tags.appliance,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "dictionary.token",
        enum = {program_failed_status_enum.ok, program_failed_status_enum.failed},
        value = {
            [program_name_enum.target_temperature_setting] = program_failed_status_enum.ok,
            [program_name_enum.water_supply] = program_failed_status_enum.ok,
            [program_name_enum.boiling] = program_failed_status_enum.ok,
            [program_name_enum.washing] = program_failed_status_enum.ok,
            [program_name_enum.rinsing] = program_failed_status_enum.ok,
            [program_name_enum.draining] = program_failed_status_enum.ok,
            [program_name_enum.spinning] = program_failed_status_enum.ok,
            [program_name_enum.drying] = program_failed_status_enum.ok,
            [program_name_enum.fan] = program_failed_status_enum.ok,
            [program_name_enum.compressor] = program_failed_status_enum.ok
        }
    },
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

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

