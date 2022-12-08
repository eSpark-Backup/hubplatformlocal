local tests = require( "tests" )
local core = require( "core" )

local function cluster_ias_zone_generator(zone_type)
    return { 
        name = "ias_zone",
        attributes = {
            {
                name = "ZoneType",
                formatted_value = {zone_type = zone_type}
            }
        }
    }
end

local endpoint_1 = { 
    id = 1,
    clusters = { cluster_ias_zone_generator("motion_sensor") }
}

local endpoint_2 = { 
    id = 2,
    clusters = { cluster_ias_zone_generator("fire_sensor") }
}

local endpoint_3 = { 
    id = 3,
    clusters = { cluster_ias_zone_generator("gas_sensor") }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Combo Sensor",
    security = 0,
    is_reachable = 1,
    is_battery_powered = false,

    endpoints = { endpoint_1, endpoint_2, endpoint_3 }
}

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

local zigbee = require( "zigbee" )

-- prepare zigbee
zigbee.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 42 
} )

-- check result reachable
core.update_reachable_state = tests.create_mock()

events_handler( {
    event = "reachable_state_updated",
    node_id = 42,
    value = true
} )

tests.assert_called( 
    core.update_reachable_state, 
    "check if reachable state updated", 
    function(call_index, device_id, value)
        tests.assert_eq(value, true, "reachable state")
    end,
    3
)

-- check result unreachable
core.update_reachable_state = tests.create_mock()

events_handler( {
    event = "reachable_state_updated",
    node_id = 42,
    value = false
} )

tests.assert_called( 
    core.update_reachable_state, 
    "check if reachable state updated", 
    function(call_index, device_id, value)
        tests.assert_eq(value, false, "reachable state")
    end,
    3
)
