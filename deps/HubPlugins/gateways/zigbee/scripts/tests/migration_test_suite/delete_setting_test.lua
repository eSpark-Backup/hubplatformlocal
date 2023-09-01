local tests = require( "tests" )
local core = require("core")
local storage = require( "storage" )

local default_changes = loadfile("HUB:zigbee/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true
loadfile = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

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
            name = "LocalTemperatureCalibration",
            formatted_value = 0
        },
        {
            name = "OccupiedCoolingSetpoint",
            formatted_value = 0
        },
        {
            name = "OccupiedHeatingSetpoint",
            formatted_value = 0
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
local thermostat_mode_enum = require("HUB:zigbee/scripts/resources/enums/thermostat_mode")

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
    }
}

local expected_settings = {
    {
        name = "thermostat_temperature_calibration",
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "LocalTemperatureCalibration",
        label = {
            lang_tag = "zigbee_device_setting_temperature_calibration_label",
            text = "Zigbee device temperature calibration"
        },
        description = {
            lang_tag = "zigbee_device_setting_temperature_calibration_description",
            text = "Value to calibrate sensor temperature"
        },
        value_type = "temperature",
        value = {
            value = 0,
            scale = "celsius"
        },
        value_default = {
            value = 0,
            scale = "celsius"
        },
        value_min = {
            value = -2.5,
            scale = "celsius"
        },
        value_max = {
            value = 2.5,
            scale = "celsius"
        }
    }
}


local expected_devices = {
    {
        name = "Thermostat",
        category = "hvac",
        subcategory = "hvac",
        items = expected_items,
        settings = expected_settings,
        node_id = 42,
        battery_powered = false
    }
}

local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices )

-- delete LocalTemperatureCalibration attribute
cluster_meter.attributes[3].name = "test"
expected_devices[1].settings = {}

-- execute migration
loadfile("HUB:zigbee/scripts/tests/migration_test_suite/run_migration")()

-- check migration result
loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices )
