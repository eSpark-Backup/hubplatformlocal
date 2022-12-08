local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local tests = require "tests"
local core = require( "core" )

zwave.request_node_values = tests.create_mock()

local node = {
    id = "9000",
    manufacturer_id = 999,
    product_type = 0,
    product_id = 0,
    is_battery_powered = false,
    is_smart_include = true,

    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",

    security = { "s0" },
    security_class_started = { "s0" },

    channels = { }
}

zwave.add_node( node )


local zwave_node_id = "9000"
local device_id = "100500"

zwave.get_provisioning_info = function(node_id)
    return { node_id = node_id, name = "the best device ever", room = "the best roomId ever" }
end

local event_filter_rule = {
    er = {
        id = device_id,
        event = "device_updated",
        accepts = { { _id = device_id, name = {} }, { _id = device_id, roomId = {} } }
    },
    ar = {}
}
core.set_subscription_filters = function(subscriber_id, filter_rules)
    event_filter_rule.ar = filter_rules[1] --indexing starts from 1
end


local added_device = { 
    er = {
        name = "the best device ever",
        roomId = "the best roomId ever",
        info = { 
            ["zwave.smartstart"] = "yes",
            ["zwave.node"] = tostring(zwave_node_id)
        }
    },
    ar = {} 
}
core.add_device = function(device)
    added_device.ar = device
    return device_id
end


-- add node
events_handler( {
    event = "node_added",
    node_id = zwave_node_id 
} )

tests.assert_eq( added_device.ar.name, added_device.er.name )
tests.assert_eq( added_device.ar.roomId, added_device.er.roomId )
tests.assert_eq( added_device.ar.info["zwave.smartstart"], added_device.er.info["zwave.smartstart"] )
tests.assert_eq( added_device.ar.info["zwave.node"], added_device.er.info["zwave.node"] )

tests.assert_eq( event_filter_rule.ar.id, event_filter_rule.er.id )
tests.assert_eq( event_filter_rule.ar.event, event_filter_rule.er.event )
tests.assert_eq( event_filter_rule.ar.accepts, event_filter_rule.er.accepts )
