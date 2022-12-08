local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require("zwave")
local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local cc_basic = {
    id = class_ids.basic,
}

local cc_notification = {
    id = class_ids.notification,
    v1_supported = false,
    notifications = {
        {
            event_types = {
                "state_idle",
                "water_leak_detected"
            },
            type = "water_alarm",
            unknown_event_types =  {}
        },
    }
}

local channel_0 = {
    id = 0,
    classes = { cc_notification, cc_basic }
}

local node = {
    id = "9000",
    manufacturer_id = 132,
    product_type = 115,
    product_id = 257,

    is_battery_powered = true,

    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- send notification event
events_handler( {
    event = "value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.basic,
    value = 0xFF,
} )

-- send motion basic event
local function find_item( item_name, item_tag ) 
   return core.get_item(storage:call("find_item_id", item_name, 9000, class_ids.notification, 0, item_tag  ) )
end

local item_names = require("HUB:zwave/scripts/resources/item_names")

local water_alarm_item = find_item( item_names.water_leak_alarm, "water_alarm" )
tests.assert_eq( water_alarm_item.value, "water_leak_detected", "check motion value after notiication value changing" )
