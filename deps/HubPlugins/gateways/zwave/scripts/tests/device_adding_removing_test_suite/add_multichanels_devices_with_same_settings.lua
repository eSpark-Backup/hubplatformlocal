local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local tests = require("tests")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

zwave.request_node_values = tests.create_mock()

local node = {
    channels = {
        {
            classes = {
                {
                    id = 67,
                    setpoints = {
                        {
                            max_scale = 0,
                            max_value = 36.4,
                            min_scale = 0,
                            min_value = 10,
                            type = 1
                        }
                    },
                    version = 3
                }
            },
            id = 0
        },
        {
            classes = {
                {
                    id = 67,
                    setpoints = {
                        {
                            max_scale = 0,
                            max_value = 36.4,
                            min_scale = 0,
                            min_value = 10,
                            type = 1
                        }
                    },
                    version = 3
                }
            },
            id = 1
        }
    },
    generic_type = "GENERIC_TYPE_THERMOSTAT",
    icon_type = "ICON_TYPE_GENERIC_THERMOSTAT",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 0,
    product_id = 1,
    product_type = 2,
    security = {
        "authenticated"
    },
    security_class_started = {
        "authenticated"
    },
    specific_type = "SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2"
}

fixture:add_node(node)

local items = {
    {
        name = "thermostat_setpoint_heating",
        cc_id = class_ids.thermostat_setpoint,
        channel_id = 0,
        is_merged = true
    },
    {
        name = "thermostat_setpoint_heating",
        cc_id = class_ids.thermostat_setpoint,
        channel_id = 1
    },
    {
        name = "thermostat_setpoint",
        cc_id = class_ids.thermostat_setpoint,
        channel_id = 1
    },
    {
        name = "thermostat_setpoint",
        cc_id = class_ids.thermostat_setpoint,
        channel_id = 0,
        is_merged = true
    },
    {
        name = "thermostat_setpoint_cooling",
        cc_id = class_ids.thermostat_setpoint,
        channel_id = 1
    },
    {
        name = "thermostat_setpoint_cooling",
        cc_id = class_ids.thermostat_setpoint,
        channel_id = 0,
        is_merged = true
    },
    {
        name = "thermostat_energy_saving_mode",
        cc_id = class_ids.thermostat_setpoint,
        channel_id = 1
    },
    {
        name = "thermostat_energy_saving_mode",
        cc_id = class_ids.thermostat_setpoint,
        channel_id = 0,
        is_merged = true
    }
}

-- check adding result
local expected_devices = {
    {
        name = "Thermostat",
        type = "device",
        category = "hvac",
        subcategory = "hvac",
        items = items,
        node_id = "9000",
        battery_powered = false,
        security = "authenticated"
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
