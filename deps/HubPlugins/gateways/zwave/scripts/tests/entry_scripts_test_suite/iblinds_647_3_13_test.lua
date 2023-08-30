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

local function check_set_item_value_result(value, switch_value)
    tests.assert_called(
        zwave.set_value, 
        "check zwave value set",
        { { "9000", class_ids.dimmer, 0, value } }
    )

    tests.assert_eq(core.get_item(dimmer_id).value, value, "Check dimmer value")
    tests.assert_eq(core.get_item(switch_id).value, switch_value, "Check switch value")

    tests.assert_called(timer.cancel, "check timer cancelled", { { "iblinds_9000_38" } })
    tests.assert_called(
        timer.set_timeout_with_id, 
        "check timer started", 
        { 
            { 
                120000, 
                "iblinds_9000_38", 
                "HUB:zwave/scripts/timer_handlers/common/request_value",
                {
                    node_id = "9000",
                    channel_id = 0,
                    class_id = class_ids.dimmer
                }
            } 
        })
end

do
    set_item_value( {           
        item_id = dimmer_id,
        value = 10
    } )
    check_set_item_value_result(10, true)
end

do
    set_item_value( {           
        item_id = dimmer_id,
        value = 0
    } )
    check_set_item_value_result(0, false)
end

do
    set_item_value( {           
        item_id = dimmer_id,
        value = 50
    } )
    check_set_item_value_result(50, true)
end

do
    set_item_value( {           
        item_id = dimmer_id,
        value = 99
    } )
    check_set_item_value_result(99, true)
end

do
    set_item_value( {           
        item_id = switch_id,
        value = true
    } )
    check_set_item_value_result(50, true)
end

do
    set_item_value( {           
        item_id = switch_id,
        value = false
    } )
    check_set_item_value_result(0, false)
end

