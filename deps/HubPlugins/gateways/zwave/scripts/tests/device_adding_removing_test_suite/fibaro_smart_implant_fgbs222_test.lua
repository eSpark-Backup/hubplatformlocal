local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/fibaro_smart_implant_fgbs222_fixture")()
local zwave = require("zwave")
local tests = require( "tests" )

zwave.request_node_values = tests.create_mock()

local node = fixture:create_base_node()

fixture:add_node(node)

local expected_devices = fixture:create_base_expected_devices()

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
