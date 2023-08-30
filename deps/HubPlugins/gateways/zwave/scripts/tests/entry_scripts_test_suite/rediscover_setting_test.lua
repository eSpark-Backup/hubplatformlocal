local tests = require( "tests" )
local zwave = require( "zwave" )
local core = require("core")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_names = require("HUB:zwave/scripts/resources/item_names")

local set_setting_value = loadfile( "HUB:zwave/scripts/set_setting_value" )

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()
zwave.start_rediscover = tests.create_mock()

local cc_binary_sensor = { 
    id = class_ids.sensor_binary,
    sensor_types = { 0x03, 0x05 }
}

local channel_0 = {
    id = 0,
    classes = { cc_binary_sensor }
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

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local setting_id = storage:call("find_setting_id", 9000, nil, "rediscover")

do
    set_setting_value({ 
        setting_id = setting_id,
        value = { }
    })

    tests.assert_called(zwave.start_rediscover, "check start rediscover called", { {
            "9000", "HUB:zwave/configs/device_setting.json"
        } } )
end
