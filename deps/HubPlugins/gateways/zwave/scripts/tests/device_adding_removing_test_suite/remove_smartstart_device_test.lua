local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local tests = require "tests"
local core = require( "core" )

zwave.request_node_values = tests.create_mock()

local zwave_node_id = 1

local node = {
    id = 1,
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


local device_id = nil

zwave.get_provisioning_info = function(node_id)
    return { node_id = node_id, name = "the best device ever", room = "the best roomId ever" }
end

local event_filter_rule = {
    er = {
        id = device_id,
        event = ""
    },
    ar = {}
}
core.set_subscription_filters = function(subscriber_id, filter_rules)
    event_filter_rule.ar = filter_rules[1] --indexing starts from 1
end

local real_core_add_device = core.add_device
core.add_device = function(device)
    device_id = real_core_add_device(device)
    return device_id
end

-- add node
events_handler( {
    event = "node_added",
    node_id = zwave_node_id 
} )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = zwave_node_id,     
    manufacturer_id = 999,
    product_type = 0,
    product_id = 0,
} )

--set a rule id to a proper value after we've known it during core device adding 
event_filter_rule.er.id = device_id

tests.assert_eq( event_filter_rule.ar.id, event_filter_rule.er.id )
tests.assert_eq( event_filter_rule.ar.event, event_filter_rule.er.event )
