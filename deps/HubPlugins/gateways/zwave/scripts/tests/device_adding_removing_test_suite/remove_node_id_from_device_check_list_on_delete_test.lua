local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { 
        {
            id = class_ids.switch_color,
            version = 2,
            component_ids = { "red", "green", "blue", "wwhite" }
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

storage:call("insert_device_checking_id", 9000, "device_id_test")


-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )

-- check removing result
tests.assert_eq(storage:call("find_device_checking_id", 9000), nil)
