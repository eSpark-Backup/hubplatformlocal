local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local tests = require("tests")
local core = require("core")

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

local function send_notification( event_type, --[[opt]]value )
    events_handler( {
        event = "notification_recieved",
        node_id = 9000,
        class_id = class_ids.notification,
        channel_id = 0,
        v1_alarm_type = 0,
        v1_alarm_level = 0,
        event_type = event_type,
        value = value,
        notification_type = "appliance"
    } )
end

local events = {
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

for _, event_name in ipairs(events) do
    core.update_item_dictionary_value = tests.create_mock()

    send_notification(event_name)

    tests.assert_called(
        core.update_item_dictionary_value, 
        "check if value_updated", 
        function(call_index, item_id, key, value)
            tests.assert_eq(value, program_failed_status_enum.failed, "Check if value corect")
        end, 
        1
    )
end

for _, event_name in ipairs(events) do
    core.update_item_dictionary_value = tests.create_mock()

    send_notification("state_idle", event_name)

    tests.assert_called(
        core.update_item_dictionary_value, 
        "check if value_updated", 
        nil, 
        10
    )
end

core.update_item_dictionary_value = tests.create_mock()

send_notification("state_idle")

tests.assert_called(
    core.update_item_dictionary_value, 
    "check if value_updated", 
    function(call_index, item_id, key, value)
        tests.assert_eq(value, program_failed_status_enum.ok, "Check if value corect")
    end, 
    10
)
