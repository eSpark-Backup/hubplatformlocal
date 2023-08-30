local tests = require( "tests" )
local zwave = require( "zwave" )
local core = require("core")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { { id = class_ids.switch_binary }, }
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

-- make device broken
local function callback(device)
    core.update_device_status(device.id, "broken")
end
hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/for_each_device")(9000, callback)

core.update_item_value = tests.create_mock()

events_handler( {
    event = "value_updated",
    node_id = 9000,
    class_id = class_ids.switch_binary,
    channel_id = 0,
    value = 0xFF
} )

tests.assert_called(core.update_item_value, "no item update", nil, 0)
