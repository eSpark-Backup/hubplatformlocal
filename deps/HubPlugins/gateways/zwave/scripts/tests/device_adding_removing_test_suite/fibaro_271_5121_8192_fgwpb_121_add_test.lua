local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local switchCommandClass = {
    id = 0x25
}

local meterCommandClass = {
    id = 0x32,
    reset_supported = true,
    type = "electric",
    scales = { "kilo_watt_hour", "watt" }
}

local notificationCommandClass = {
    id = 0x71,
    notifications = {
        {
            type = "power_management",
            event_types = {
                "over_current_detected",
                "over_load_detected"
            }
        },
        {
            type = "system",
            event_types = {
                "system_software_failure"
            }
        }
    }
}

local node = {
    id = "9000",
    channels =  {
        {
            id = 0,
            classes = {
                switchCommandClass,
                meterCommandClass,
                notificationCommandClass
            }
        },
        {
            id = 1,
            classes = {
                switchCommandClass,
                meterCommandClass,
                notificationCommandClass
            }
        },
        {
            id = 2,
            classes = {
                meterCommandClass
            }
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
    is_battery_powered = false,
    manufacturer_id = 271,
    product_type = 5121,
    product_id = 8192,
    security =  { "s0" },
    security_class_started =  { "s0" }
}

fixture:add_node(node)

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    tests = tests
}

local expected_devices = {
    {
        name = "Wall Plug",
        type = "switch.outlet",
        category = "switch",
        subcategory = "interior_plugin",
        items = {
            { channel_id = 0, cc_id = 0x25, name = "switch" },
            { channel_id = 0, cc_id = 0x32, name = "electric_meter_kwh" },
            { channel_id = 0, cc_id = 0x32, name = "electric_meter_watt" },
            { channel_id = 0, cc_id = 0x32, name = "meter_reset" },
            { channel_id = 0, cc_id = 0x71, name = "sw_state", tag = "system" },
            { channel_id = 0, cc_id = 0x71, name = "over_current_state", tag = "power_management" },
            { channel_id = 0, cc_id = 0x71, name = "over_load_state", tag = "power_management" },

            { channel_id = 1, cc_id = 0x25, name = "switch", is_merged = true },
            { channel_id = 1, cc_id = 0x32, name = "electric_meter_kwh", is_merged = true },
            { channel_id = 1, cc_id = 0x32, name = "electric_meter_watt", is_merged = true },
            { channel_id = 1, cc_id = 0x32, name = "meter_reset", is_merged = true },
            { channel_id = 1, cc_id = 0x71, name = "sw_state", tag = "system", is_merged = true },
            { channel_id = 1, cc_id = 0x71, name = "over_current_state", tag = "power_management", is_merged = true },
            { channel_id = 1, cc_id = 0x71, name = "over_load_state", tag = "power_management", is_merged = true }
        },
        node_id = "9000",
        battery_powered = false,
        security = "middle",
        subdevices = {
            {
                name = "Wall Plug 1",
                type = "meter.power",
                category = "power_meter",
                subcategory = "",
                items = {
                    { channel_id = 2, cc_id = 0x32, name = "electric_meter_kwh" },
                    { channel_id = 2, cc_id = 0x32, name = "electric_meter_watt" },
                    { channel_id = 2, cc_id = 0x32, name = "meter_reset" }
                }
            }
        }
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
