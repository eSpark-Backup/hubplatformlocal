local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local function make_channel(id)
    return {
        id = id,
        classes = {
            { id = 0x26 } -- This test check for device names, not items created
        }
    }
end

local node = {
    id = "9000",
    channels =  {
        make_channel(0),
        make_channel(1),
        make_channel(2),
        make_channel(3),
        make_channel(4),
        make_channel(5),
    },
    generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL",
    is_battery_powered = false,
    manufacturer_id = 271,
    product_type = 2304,
    product_id = 4096,
    security =  { "none" },
    security_class_started =  { "none" }
}

fixture:add_node(node)

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    tests = tests
}

local function make_device(channel_id, name, subdevices)
    return {
        name = name,
        type = "light.bulb",
        category = "dimmable_light",
        subcategory = "dimmable_colored",
        items = {
            { channel_id = channel_id, cc_id = 0x26, name = "switch" },
            { channel_id = channel_id, cc_id = 0x26, name = "dimmer" },
            { channel_id = channel_id, cc_id = 0x26, name = "dimmer_up" },
            { channel_id = channel_id, cc_id = 0x26, name = "dimmer_down" },
            { channel_id = channel_id, cc_id = 0x26, name = "dimmer_stop" },
        },
        node_id = "9000",
        battery_powered = false,
        security = "none",
        subdevices = subdevices or {}
    }
end

local expected_devices = {
    make_device(0, "Fibaro RGBW Controller", {
        make_device(1, "Brightness"),
        make_device(2, "Red"),
        make_device(3, "Green"),
        make_device(4, "Blue"),
        make_device(5, "White"),
    })
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
