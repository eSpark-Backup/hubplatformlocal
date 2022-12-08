local tests = require( "tests" )

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local set_item_value = loadfile("HUB:zwave/scripts/set_item_value")

local timer = require "timer"

timer.set_timeout_with_id = tests.create_mock()
timer.cancel = tests.create_mock()
zwave.request_node_values = tests.create_mock()
zwave.set_value = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { 
        { id = class_ids.battery },
        { id = class_ids.switch_binary },
        { id = class_ids.dimmer }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 647,
    product_type = 3,
    product_id = 13,
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


local switch_id = storage:call("find_item_id", item_names.switch, 9000, class_ids.switch_binary, 0)
local dimmer_id = storage:call("find_item_id", item_names.dimmer, 9000, class_ids.dimmer, 0)

do
    events_handler( {
        event = "value_updated",
        node_id = 9000,
        class_id = class_ids.dimmer,
        channel_id = 0,
        value = 1
    } )

    tests.assert_eq(core.get_item(dimmer_id).value, 1, "Check dimmer value")
    tests.assert_eq(core.get_item(switch_id).value, true, "Check switch value")
end

do
    events_handler( {
        event = "value_updated",
        node_id = 9000,
        class_id = class_ids.dimmer,
        channel_id = 0,
        value = 0
    } )

    tests.assert_eq(core.get_item(dimmer_id).value, 0, "Check dimmer value")
    tests.assert_eq(core.get_item(switch_id).value, false, "Check switch value")
end

do
    events_handler( {
        event = "value_updated",
        node_id = 9000,
        class_id = class_ids.switch_binary,
        channel_id = 0,
        value = 0
    } )

    tests.assert_eq(core.get_item(dimmer_id).value, 0, "Check dimmer value")
    tests.assert_eq(core.get_item(switch_id).value, false, "Check switch value")
end

do
    events_handler( {
        event = "value_updated",
        node_id = 9000,
        class_id = class_ids.switch_binary,
        channel_id = 0,
        value = 0xFF
    } )

    tests.assert_eq(core.get_item(dimmer_id).value, 50, "Check dimmer value")
    tests.assert_eq(core.get_item(switch_id).value, true, "Check switch value")
end
