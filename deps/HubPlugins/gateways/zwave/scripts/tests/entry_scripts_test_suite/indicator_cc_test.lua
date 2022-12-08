local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local set_setting_value = loadfile( "HUB:zwave/scripts/set_setting_value" )

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { 
        {
            id = class_ids.indicator,
            version = 3
        },
        {
            id = class_ids.switch_binary,
        }
    }
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

local setting_id = 
    assert(storage:call("find_setting_id", 9000, class_ids.indicator, "identify"), "setting id with tag 'identify' must be present")

-- try set setting value
zwave.send_identify = tests.create_mock()

set_setting_value({ 
    setting_id = setting_id
})

tests.assert_called(zwave.send_identify, "check send_identify was called", {{ "9000", 0 }})
