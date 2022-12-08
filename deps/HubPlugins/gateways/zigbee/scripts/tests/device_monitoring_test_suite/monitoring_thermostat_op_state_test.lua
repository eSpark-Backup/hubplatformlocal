local tests = require( "tests" )
local core = require( "core" )

local cluster_meter = {
    name = "thermostat",
    attributes = {
        {
            name = "SystemMode",
            formatted_value = "off"
        },
        {
            name = "LocalTemperature",
            formatted_value = 0
        },
        {
            name = "OccupiedCoolingSetpoint",
            formatted_value = 0
        },
        {
            name = "OccupiedHeatingSetpoint",
            formatted_value = 0
        },
        {
            name = "ThermostatRunningState"
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_meter }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Thermostat",
    security = 0,
    is_reachable = 1,
    is_battery_powered = false,

    endpoints = { endpoint_1 }
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

local storage = hubplugins.object( "storage_helper", "HUB:zigbee/scripts/utils" )
local item_id = storage:call(
    "find_item_id",
    {
        item_name = "thermostat_operating_state",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "ThermostatRunningState"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

local function check_item_updated(attr_value, new_item_value)
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute = {
            name = "ThermostatRunningState",
            formatted_value = attr_value
        }
    } )

    tests.assert_called( core.update_item_value, "check if item values updated", {{ item_id, new_item_value }} )
end

-- single state in attr
check_item_updated( nil, "idle" )
check_item_updated( {"cool"}, "cooling" )
check_item_updated( {"heat"}, "heating" )
check_item_updated( {"fan"}, "fan_only" )
check_item_updated( {"heat_2nd_stage"}, "2nd_stage_heating" )
check_item_updated( {"cool_2nd_stage"}, "2nd_stage_cooling" )
check_item_updated( {"fan_2nd_stage"}, "2nd_stage_fan" )
check_item_updated( {"fan_3rd_stage"}, "3rd_stage_fan" )

--several states in attr
check_item_updated( {"cool", "fan", "fan_2nd_stage", "fan_3rd_stage"}, "cooling" )
check_item_updated( {"heat", "fan", "fan_2nd_stage", "fan_3rd_stage"}, "heating" )

check_item_updated( {"cool_2nd_stage", "fan", "fan_2nd_stage", "fan_3rd_stage"}, "2nd_stage_cooling" )
check_item_updated( {"heat_2nd_stage", "fan", "fan_2nd_stage", "fan_3rd_stage"}, "2nd_stage_heating" )

check_item_updated( {"cool", "cool_2nd_stage"}, "cooling" )
check_item_updated( {"heat", "heat_2nd_stage"}, "heating" )
check_item_updated( {"fan", "fan_2nd_stage", "fan_3rd_stage"}, "fan_only" )
