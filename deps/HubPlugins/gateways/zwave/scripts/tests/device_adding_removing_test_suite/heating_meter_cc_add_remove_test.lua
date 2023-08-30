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
            reset_supported = true,
            type = "heating",
            scales = {
                "kilo_watt_hour"
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
        name = "Heater",
        category = "hvac",
        subcategory = "heater",
        items = {
            {
                name = item_names.heating_meter_energy,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.energy,
                value = {
                    value = 0,
                    scale = "kilo_watt_hour"
                }
            },
            {
                name = item_names.meter_reset,
                cc_id = class_ids.meter,
                channel_id = 0,
                value_type = item_value_type.int,
            }
        },
        node_id = "9000",
        battery_powered = true,
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

local check_context = {}

hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

fixture:remove_node(node)

hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
