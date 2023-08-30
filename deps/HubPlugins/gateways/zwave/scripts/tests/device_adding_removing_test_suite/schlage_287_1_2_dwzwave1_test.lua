local fixture = require("HUB:zwave/scripts/tests/fixtures/base_fixture")
local zwave = require("zwave")
local tests = require("tests")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

zwave.request_node_values = tests.create_mock()

local notification_command_class = {
    id = 0x71,
    version = 2,
    notifications = {
        {
            type = "home_security",
            event_types = {}
        }
    }
}

local node = {
    id = "9000",
    channels =  {
        {
            id = 0,
            classes = {
                notification_command_class
            }
        }
    },
    generic_type = "GENERIC_TYPE_SENSOR_BINARY",
    specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_BINARY",
    is_battery_powered = true,
    manufacturer_id = 287,
    product_type = 1,
    product_id = 2,
    security =  { "none" },
    security_class_started =  { "none" }
}

fixture:add_node(node)

local check_context = {}

local expected_devices = {
    {
        node_id = "9000",
        name = "Door/Window Sensor",
        type = "sensor",
        category = "security_sensor",
        subcategory = "door",
        battery_powered = true,
        security = "no",
        items = {
            { channel_id = 0, cc_id = class_ids.notification, name = item_names.tampering_cover_alarm, tag = "home_security" }
        },
        subdevices = {}
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
