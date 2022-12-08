local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local core = require("core")

local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = {
        {
            id = class_ids.meter,
            reset_supported = false,
            type = "electric",
            scales = {
                "kilo_watt_hour",
                "kilo_volt_ampere_hour",
                "watt",
                "pulse_count",
                "volt",
                "ampere",
                "power_factor",
                "kilo_volt_ampere_reactive",
                "kilo_volt_ampere_reactive_hour",
            }
        },
    }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "Unknown",
        category = "unknown",
        items = {
            {
                name = item_names.electric_meter_kwh,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.amount_of_useful_energy,
                value = {
                    value = 0,
                    scale = "kilo_watt_hour"
                }
            },
            {
                name = item_names.electric_meter_kvah,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.kilo_volt_ampere_hour,
                value = {
                    value = 0,
                    scale = "kilo_volt_ampere_hour"
                }
            },
            {
                name = item_names.electric_meter_watt,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.power,
                value = {
                    value = 0,
                    scale = "watt"
                }
            },
            {
                name = item_names.electric_meter_pulse,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.int,
                value = 0
            },
            {
                name = item_names.electric_meter_volt,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.electric_potential,
                value = {
                    value = 0,
                    scale = "volt"
                }
            },
            {
                name = item_names.electric_meter_amper,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.electric_current,
                value = {
                    value = 0,
                    scale = "ampere"
                }
            },
            {
                name = item_names.electric_meter_factor,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.general_purpose,
                value = {
                    value = 0,
                    scale = "float"
                }
            },
            {
                name = item_names.electric_meter_kvar,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.reactive_power_instant,
                value = {
                    value = 0,
                    scale = "kilo_volt_amper_reactive"
                }
            },
            {
                name = item_names.electric_meter_kvarh,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.reactive_power_consumption,
                value = {
                    value = 0,
                    scale = "kilo_volt_amper_reactive_hour"
                }
            }
        },
        node_id = "9000",
        battery_powered = true,
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

local check_context = {}

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

fixture:remove_node(node)

loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
