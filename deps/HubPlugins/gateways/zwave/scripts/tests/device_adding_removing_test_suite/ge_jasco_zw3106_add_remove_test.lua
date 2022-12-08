local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local fixture = hubplugins.weak_loadfile( "HUB:zwave/scripts/tests/fixtures/ge_jasco_zw3106_fixture" )()
local zwave = require("zwave")
local tests = require( "tests" )

zwave.request_node_values = tests.create_mock()

local expected_devices = {
    {
        name = "Plug-in Dual Smart Dimmer",
        category = "dimmable_light",
        subcategory = "dimmable_plugged",
        items = {
            { name = "dimmer_up", cc_id = class_ids.dimmer, channel_id = 0 },
            { name = "dimmer_stop", cc_id = class_ids.dimmer, channel_id = 0 },
            { name = "switch", cc_id = class_ids.dimmer, channel_id = 0 },
            { name = "dimmer", cc_id = class_ids.dimmer, channel_id = 0 },
            { name = "dimmer_down", cc_id = class_ids.dimmer, channel_id = 0 },
        },
        node_id = "9000",
        battery_powered = false,
        subdevices = {
            {
                name = "Plug-in Dual Smart Dimmer 1",
                category = "dimmable_light",
                subcategory = "dimmable_plugged",
                items = {
                    { name = "dimmer_up", cc_id = class_ids.dimmer, channel_id = 1 },
                    { name = "dimmer_stop", cc_id = class_ids.dimmer, channel_id = 1 },
                    { name = "switch", cc_id = class_ids.dimmer, channel_id = 1 },
                    { name = "dimmer", cc_id = class_ids.dimmer, channel_id = 1 },
                    { name = "dimmer_down", cc_id = class_ids.dimmer, channel_id = 1 },
                }
            },
            {
                name = "Plug-in Dual Smart Dimmer 2",
                category = "dimmable_light",
                subcategory = "dimmable_plugged",
                items = {
                    { name = "dimmer_up", cc_id = class_ids.dimmer, channel_id = 2 },
                    { name = "dimmer_stop", cc_id = class_ids.dimmer, channel_id = 2 },
                    { name = "switch", cc_id = class_ids.dimmer, channel_id = 2 },
                    { name = "dimmer", cc_id = class_ids.dimmer, channel_id = 2 },
                    { name = "dimmer_down", cc_id = class_ids.dimmer, channel_id = 2 },
                }
            },
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    tests = tests
}

local node = fixture:create_node("9000")

fixture:add_node(node)

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)