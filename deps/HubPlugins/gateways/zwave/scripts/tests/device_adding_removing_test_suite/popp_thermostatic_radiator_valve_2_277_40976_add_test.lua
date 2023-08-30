local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    classes = {
        {
            id = 32,
            is_supported = false,
            version = 0
        },
        {
            id = 112,
            parameters = {},
            version = 0
        },
        {
            id = 128,
            version = 1
        },
        {
            id = 129,
            version = 1
        },
        {
            id = 114,
            version = 1
        },
        {
            id = 49,
            sensors = {
                {
                    scales = {
                        "celsius"
                    },
                    sensor_type = "air_temperature"
                }
            },
            version = 6
        },
        {
            id = 67,
            setpoints = {
                {type = 1}
            },
            version = 2
        },
        {
            id = 134,
            firmware_versions = {
                "1.0"
            },
            hardware_version = 0,

            library_type = "routing_slave",
            version = 1,
            zwave_protocol_version = "3.67"
        },
        {
            id = 132,
            version = 2
        }
    },
    id = 0
}

local node = {
    id = "9000",
    manufacturer_id = 2,
    product_type = 277,
    product_id = 40976,
    is_battery_powered = true,
    generic_type = "GENERIC_TYPE_THERMOSTAT",
    specific_type = "SPECIFIC_TYPE_UNKNOWN",

    security = {"none"},
    security_class_started = {"none"},
    wake_up_interval = 1800,
    channels = {channel_0}
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "Thermostat",
        category = "hvac",
        subcategory = "hvac",
        items = {
            {
                name = "thermostat_setpoint",
                cc_id = class_ids.thermostat_setpoint,
                channel_id = 0
            },
            {
                name = "thermostat_setpoint_heating",
                cc_id = class_ids.thermostat_setpoint,
                channel_id = 0
            },
            {
                name = "thermostat_mode",
                cc_id = class_ids.thermostat_mode,
                channel_id = 0,
                value_type = "token",
                enum = {
                    "heat"
                },
                value = "heat"
            },
            {
                name = "battery",
                cc_id = class_ids.battery,
                channel_id = 0
            },
            {
                name = "thermostat_energy_saving_mode",
                cc_id = class_ids.thermostat_setpoint,
                channel_id = 0,
                value_type = "token",
                enum = {
                    "aggressive_postponed",
                    "low_postponed",
                    "off",
                    "low",
                    "aggressive",
                    "moderate",
                    "moderate_postponed"
                }
            },
            {
                name = "temp",
                cc_id = class_ids.multilevel_sensor,
                channel_id = 0
            },
            {
                name = "thermostat_setpoint_cooling",
                cc_id = class_ids.thermostat_setpoint,
                channel_id = 0
            }
        },
        node_id = "9000",
        battery_powered = true
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    tests = tests
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
