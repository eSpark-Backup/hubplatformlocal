local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 112,
                    parameters = {},
                    version = 2
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    firmware_versions = {
                        "1.0",
                        "2.2"
                    },
                    hardware_version = 1,
                    id = 134,
                    library_type = "routing_slave",
                    version = 2,
                    zwave_protocol_version = "3.67"
                },
                {
                    id = 128,
                    version = 1
                },
                {
                    id = 132,
                    version = 2
                },
                {
                    id = 49,
                    sensors = {
                        {
                            scales = {},
                            sensor_type = "air_temperature"
                        }
                    },
                    version = 6
                },
                {
                    buttons = {
                        {
                            button_attributes = {
                                "press_1_time",
                                "released",
                                "held_down"
                            },
                            button_id = 1
                        }
                    },
                    id = 91,
                    version = 1
                },
                {
                    id = 67,
                    setpoints = {
                        {
                            max_scale = 1,
                            max_value = 212,
                            min_scale = 1,
                            min_value = 0,
                            type = 1
                        },
                        {
                            max_scale = 1,
                            max_value = 212,
                            min_scale = 1,
                            min_value = 0,
                            type = 2
                        },
                        {
                            max_scale = 1,
                            max_value = 212,
                            min_scale = 1,
                            min_value = 0,
                            type = 10
                        }
                    },
                    version = 3
                },
                {
                    id = 135,
                    version = 1
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_SENSOR_MULTILEVEL",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 2,
    product_id = 32784,
    product_type = 3,
    security = {
        "none"
    },
    security_class_started = {
        "none"
    },
    specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL",
    wake_up_interval = 5400
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "Danfoss RS Radiator",
        category = "hvac",
        subcategory = "heater",
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
                enum = {"heat", "off"},
                value = "heat"
            },
            {
                name = "battery",
                cc_id = class_ids.battery,
                channel_id = 0
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "1",
                value = {button_number = 1, button_state = "idle"},
                cc_id = class_ids.central_scene,
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
