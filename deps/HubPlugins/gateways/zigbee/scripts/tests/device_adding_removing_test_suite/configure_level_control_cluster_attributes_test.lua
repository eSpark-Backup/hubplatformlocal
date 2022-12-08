local tests = require( "tests" )
local zigbee = require( "zigbee" )

local cluster_level_control = { 
    name = "level_control",
    attributes = {
        {
            name = "CurrentLevel",
            formatted_value = 0
        }
    }
}

local endpoint_1 = { 
    id = 1,
    clusters = { cluster_level_control }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Bulb",
    security = 0,
    is_reachable = 1,
    is_battery_powered = false,

    endpoints = { endpoint_1 }
}

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

local zigbee = require( "zigbee" )

-- prepare zigbee
zigbee.add_node( node )

zigbee.configure_attribute_report = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

-- add node
events_handler( {
    event = "node_added",
    node_id = 42 
} )

tests.assert_called( 
    zigbee.configure_attribute_report, 
    "check if attribute report configured", 
    {{ 42, 1, "level_control", "CurrentLevel", 0x0001, 0x012C, 1 }} 
)

tests.assert_called( 
    zigbee.request_attribute_value, 
    "check if attribute value requested", 
    {{ 42, 1, "level_control", "CurrentLevel" }} 
)
