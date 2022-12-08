local tests = require( "tests" )
local zwave = require("zwave")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/dw_sensor_265_8193_258_fixture")()

zwave.request_node_values = tests.create_mock()

local expected_items = {
    {
        name = item_names.battery,
        cc_id = class_ids.battery,
        channel_id = 0,
    },
    {
        name = item_names.basic,
        cc_id = class_ids.basic,
        channel_id = 0,
    },
    {
        name = item_names.tampering_cover_alarm,
        cc_id = class_ids.notification,
        channel_id = 0,
        tag = item_descriptor_tags.home_security,
    },
    {
        name = item_names.dw_state,
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        tag = item_descriptor_tags.access_control,
    },
    {
        name = "security_threat",
        tag = "access_control.security_tag",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        main_item_name = item_names.dw_state
    }
}

local expected_devices = {
    {
        name = "Door & Window Sensor",
        category = "security_sensor",
        subcategory = "door",
        items = expected_items,
        node_id = fixture.node_id_str,
        battery_powered = true,
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local node = fixture:create_node()

fixture:add_node(node)

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

fixture:remove_node(node)

loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
