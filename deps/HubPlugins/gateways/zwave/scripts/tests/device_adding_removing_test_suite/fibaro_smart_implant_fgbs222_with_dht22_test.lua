local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/fibaro_smart_implant_fgbs222_fixture")()
local zwave = require("zwave")
local tests = require( "tests" )

zwave.request_node_values = tests.create_mock()

local node = fixture:create_dht22_node()

fixture:add_node(node)

local expected_devices = fixture:create_base_expected_devices()

table.insert(expected_devices[1].subdevices, {
    name = "Temperature Sensor",
    category = "temperature",
    items = {
        {
            name = item_names.temp,
            cc_id = class_ids.multilevel_sensor,
            channel_id = 8,
        },
        {
            name = item_names.sw_state,
            cc_id = class_ids.notification,
            tag = "system",
            channel_id = 8
        },
    }
})

table.insert(expected_devices[1].subdevices, {
    name = "Humidity Sensor",
    category = "humidity",
    items = {
        {
            name = item_names.humidity,
            cc_id = class_ids.multilevel_sensor,
            channel_id = 9,
        },
        {
            name = item_names.humidity,
            cc_id = class_ids.multilevel_sensor,
            channel_id = 0,
            is_merged = true
        },
        {
            name = item_names.sw_state,
            cc_id = class_ids.notification,
            tag = "system",
            channel_id = 9
        },
    }
})


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
