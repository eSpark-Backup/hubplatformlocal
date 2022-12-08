local tests = require( "tests" )
local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

zwave.request_node_values = tests.create_mock()

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local cc_barrier = { 
    id = class_ids.barrier_operator,
}

local cc_notification = { 
    id = class_ids.notification,
    v1_supported = false,
    notifications = {
        {
            type = "access_control",
            event_types = {
                "state_idle",
                "barrier_unable_to_perform_requested_operation_due_to_ul_requirements",
                "barrier_unattended_operation_has_been_disabled_per_ul_requirements",
                "barrier_failed_to_perform_requested_operation_device_malfunction",
                "barrier_sensor_not_detected",
                "barrier_sensor_low_battery_warning"
            }
        }
    }
}

local channel_0 = { 
    id = 0,
    classes = { cc_barrier, cc_notification }
}

local node = {
    id = "9000",
    manufacturer_id = 335,
    product_type = 18244,
    product_id = 12336,
    is_battery_powered = false,

    specific_type = "SPECIFIC_TYPE_SECURE_BARRIER_ADDON",
    generic_type = "GENERIC_TYPE_ENTRY_CONTROL",

    security = { "s0" },
    security_class_started = { "s0" },

    channels = { channel_0 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000 
} )

-- check adding result
local expected_items = {
    {
        name = "barrier",
        cc_id = class_ids.barrier_operator,
        value_type = "token",
        channel_id = 0,
        enum = {"barrier_closed", "barrier_opened"},
        value = "barrier_closed"
    },
    {
        name = "barrier_fail_events",
        cc_id = class_ids.notification,
        value_type = "token",
        tag = item_descriptor_tags.access_control,
        channel_id = 0,
        enum = {
            "no_barrier_fails", 
            "barrier_unable_to_perform_requested_operation_due_to_ul_requirements", 
            "barrier_failed_to_perform_requested_operation_device_malfunction"
        },
        value = "no_barrier_fails"
    },
    {
        name = "barrier_problem_sensors",
        cc_id = class_ids.notification,
        tag = item_descriptor_tags.access_control,
        value_type = "dictionary.token",
        channel_id = 0,
        enum = {
            "low_battery", 
            "not_detected"
        },
        value = {}
    },
    {
        name = "barrier_unattended_operation",
        cc_id = class_ids.notification,
        tag = item_descriptor_tags.access_control,
        value_type = "token",
        channel_id = 0,
        enum = {
            "unattended_operation_enabled", 
            "unattended_operation_disabled_per_ul_requirements"
        },
        value = "unknown"
    },
    {
        name = "barrier_state",
        cc_id = class_ids.barrier_operator,
        value_type = "token",
        channel_id = 0,
        enum = {
            "barrier_closed", 
            "operating_unknown", 
            "barrier_opened", 
            "operating_opening", 
            "operating_closing"
        },
        value = "operating_unknown"
    }
}

local expected_devices = {
    {
        name = "Garage Door Opener",
        category = "garage_door",
        items = expected_items,
        node_id = "9000",
        battery_powered = false,
        security = "middle"
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )
