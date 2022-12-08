local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

zwave.request_node_values = tests.create_mock()

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local cc_basic = { 
    id = class_ids.basic,
}

local cc_notification = { 
    id = class_ids.notification,
    v1_supported = false,
    notifications = {
        {
            type = "access_control",
            event_types = {
                "dw_is_opened",
                "dw_is_closed"
            }
        }
    }
}

local channel_0 = { 
    id = 0,
    classes = { cc_basic, cc_notification }
}

local node = {
    id = "9000",
    manufacturer_id = 134,
    product_type = 258,
    product_id = 89,
    is_battery_powered = true,

    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",
    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",

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

core.update_item_value = tests.create_mock()

events_handler( {
    event = "value_updated",
    node_id = 9000,
    class_id = class_ids.basic,
    channel_id = 0,
    value = 255
} )

tests.assert_called(core.update_item_value, "check if item values updated", 
    function(call_index, item_id, value)
        if call_index == 1 then
            tests.assert_eq(value, "dw_is_opened", "Check new item value")
            tests.assert_eq(core.get_item( item_id ).name, "dw_state", "Check item name")
        else
            tests.assert_eq(value, true, "Check new item value")
            tests.assert_eq(core.get_item( item_id ).name, "security_threat", "Check item name")
        end
    end,
    2
)
