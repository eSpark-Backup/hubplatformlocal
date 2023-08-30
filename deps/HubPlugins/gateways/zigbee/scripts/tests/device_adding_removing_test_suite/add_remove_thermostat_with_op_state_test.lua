local tests = require( "tests" )

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

-- check adding result
local thermostat_mode_enum = loadfile("HUB:zigbee/scripts/resources/enums/thermostat_mode")()
local op_state_enum = loadfile("HUB:zigbee/scripts/resources/enums/thermostat_operating_state")()

local expected_items = {
    {
        name = "thermostat_mode",
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "SystemMode",
        value_type = "token",
        enum = {
            thermostat_mode_enum.off,
            thermostat_mode_enum.heat,
            thermostat_mode_enum.cool,
            thermostat_mode_enum.auto,
            thermostat_mode_enum.emergency_heating,
            thermostat_mode_enum.precooling,
            thermostat_mode_enum.fan_only,
            thermostat_mode_enum.dry_air,
            thermostat_mode_enum.sleep },
        value = thermostat_mode_enum.off
    },
    {
        name = "temp",
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "LocalTemperature",
        value_type = "temperature",
        value = { value = 0, scale = "celsius" }
    },
    {
        name = "thermostat_setpoint_cooling",
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "OccupiedCoolingSetpoint",
        value_type = "temperature",
        value = { value = 0, scale = "celsius" }
    },
    {
        name = "thermostat_setpoint_heating",
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "OccupiedHeatingSetpoint",
        value_type = "temperature",
        value = { value = 0, scale = "celsius" }
    },
    {
        name = "thermostat_operating_state",
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "ThermostatRunningState",
        value_type = "token",
        enum = {
            op_state_enum.idle,
            op_state_enum.heating,
            op_state_enum.cooling,
            op_state_enum.fan_only,
            op_state_enum.second_stage_heating,
            op_state_enum.second_stage_cooling,
            op_state_enum.second_stage_fan,
            op_state_enum.third_stage_fan },
        value = op_state_enum.idle
    }
}


local expected_devices = {
    {
        name = "Thermostat",
        category = "hvac",
        subcategory = "hvac",
        items = expected_items,
        node_id = 42,
        battery_powered = false
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = 42,     
    manufacturer_id = "Ezlo",
    model_id = "Thermostat"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
