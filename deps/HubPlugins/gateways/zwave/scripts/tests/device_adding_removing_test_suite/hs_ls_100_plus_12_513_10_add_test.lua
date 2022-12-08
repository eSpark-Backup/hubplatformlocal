-- Test case verifies that tampering_move_alarm item is created for HS-LS-100+ (leak sensor) even though there's a version
-- mismatch between its Notification CC version (5) and the version of "product_moved" (0x09) event type (8+)
local tests = require("tests")

local core = require("core")

local zwave = require("zwave")
zwave.request_node_values = tests.create_mock()

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local tampering_move_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_move_events")

local node = {
    id = "9000",
    manufacturer_id = 12,
    product_type = 513,
    product_id = 10,
    is_battery_powered = true,

    specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_BINARY",
    generic_type = "GENERIC_TYPE_SENSOR_BINARY",
    icon_type = "ICON_TYPE_GENERIC_SENSOR_NOTIFICATION",

    security = { "none" },
    security_class_started = { "none" },

    wake_up_interval = 3600,

    channels = { {
                     id = 0,
                     classes = {
                         {
                             id = 32,
                             is_supported = false,
                             version = 1
                         },
                         {
                             id = 112,
                             parameters = {

                             },
                             version = 1
                         },
                         {
                             id = 94,
                             version = 2
                         },
                         {
                             id = 133,
                             version = 2
                         },
                         {
                             id = 89,
                             version = 1
                         },
                         {
                             firmware_versions = {
                                 "1.3"
                             },
                             hardware_version = 1,
                             id = 134,
                             library_type = "enhanced_slave",
                             version = 2,
                             zwave_protocol_version = "4.61"
                         },
                         {
                             id = 114,
                             version = 2
                         },
                         {
                             id = 128,
                             version = 1
                         },
                         {
                             id = 159,
                             version = 1
                         },
                         {
                             id = 113,
                             notifications = {
                                 {
                                     event_types = {
                                         "overheat_detected",
                                         "under_heat_detected"
                                     },
                                     type = "heat_alarm"
                                 },
                                 {
                                     event_types = {
                                         "water_leak_detected"
                                     },
                                     type = "water_alarm"
                                 },
                                 {
                                     type = "home_security",
                                     unknown_event_types = {
                                         9
                                     }
                                 }
                             },
                             v1_supported = false,
                             version = 5
                         },
                         {
                             id = 132,
                             version = 2
                         },
                         {
                             id = 49,
                             sensors = {
                                 {
                                     sensor_type = "air_temperature"
                                 }
                             },
                             version = 5
                         },
                         {
                             id = 48,
                             sensor_types = {
                                 5,
                                 6,
                                 7,
                                 8
                             },
                             version = 2
                         },
                         {
                             firmware_ids = {
                                 2561
                             },
                             id = 122,
                             version = 3
                         }
                     }
                 } }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local function get_tampering_move_alarm_item_value()
    local item = assert( core.get_item( storage:call("find_item_id", item_names.tampering_move_alarm, 9000, class_ids.notification, 0, item_descriptor_tags.home_security ) ) )
    return item and item.value
end

tests.assert_eq(get_tampering_move_alarm_item_value(), tampering_move_enum.unknown, "check the default value of tampering_move_alarm item")

events_handler({
    event = "unknown_notification_recieved",
    node_id = 9000,
    channel_id = 0,
    class_id = class_ids.notification,
    notification_type = "home_security",
    event_type = 0x09,
    v1_alarm_level = 0,
    v1_alarm_type = 0
})

tests.assert_eq(get_tampering_move_alarm_item_value(), tampering_move_enum.product_moved, "check the updated value of tampering_move_alarm item")
