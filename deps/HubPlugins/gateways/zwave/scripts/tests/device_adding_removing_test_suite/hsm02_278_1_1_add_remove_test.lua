local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/hsm02_278_1_1_fixture")()
local zwave = require("zwave")
local tests = require( "tests" )

zwave.request_node_values = tests.create_mock()

local node = fixture:create_node("9000")

fixture:add_node(node)

local expected_devices = {
    {
        name = "Motion Sensor",
        category = "security_sensor",
        subcategory = "motion",
        node_id = "9000",
        battery_powered = true,
        items = {
            {
                name = item_names.basic,
                cc_id = class_ids.basic,
                channel_id = 0,
            },
            {
                name = item_names.battery,
                cc_id = class_ids.battery,
                channel_id = 0,
            },
            {
                name = item_names.motion,
                cc_id = class_ids.sensor_binary,
                tag = item_tags.home_security,
                channel_id = 0,
            },
            {
                name = item_names.security_threat,
                tag = "home_security.security_tag",
                cc_id = class_ids.sensor_binary,
                channel_id = 0,
                main_item_name = item_names.motion
            },
        },
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
