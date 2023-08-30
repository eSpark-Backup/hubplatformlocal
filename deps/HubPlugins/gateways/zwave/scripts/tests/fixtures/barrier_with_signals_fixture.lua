local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local cc_barrier = { 
    id = class_ids.barrier_operator,
    signals = { "audible_notification", "visual_notification" }
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
