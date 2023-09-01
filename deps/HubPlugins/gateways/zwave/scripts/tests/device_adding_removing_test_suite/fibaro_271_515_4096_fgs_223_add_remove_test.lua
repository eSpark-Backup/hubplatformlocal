local fixture = require("HUB:zwave/scripts/tests/fixtures/base_fixture")
local zwave = require("zwave")
local tests = require("tests")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

zwave.request_node_values = tests.create_mock()

local function make_channel(id)
    return {
        id = id,
        classes = {
            {
                id = class_ids.switch_binary
            }
        }
    }
end

local function test_impl(name, device_info)
    local node = {
        id = "9000",
        channels =  {
            make_channel(0),
            make_channel(1),
            make_channel(2),
        },
        generic_type = "GENERIC_TYPE_SWITCH_BINARY",
        specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
        is_battery_powered = false,
        manufacturer_id = device_info.manufacturer_id,
        product_type = device_info.product_type,
        product_id = device_info.product_id,
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

    local expected_devices = {
        {
            node_id = "9000",
            name = name .. " - 1st Switch",
            type = "switch.inwall",
            category = "switch",
            subcategory = "in_wall",
            battery_powered = false,
            security = "no",
            items = {
                { channel_id = 0, cc_id = class_ids.switch_binary, name = item_names.switch, is_merged = false },
                { channel_id = 1, cc_id = class_ids.switch_binary, name = item_names.switch, is_merged = true }
            },
            subdevices = {
                {
                    node_id = "9000",
                    name = name .. " - 2nd Switch",
                    type = "switch.inwall",
                    category = "switch",
                    subcategory = "in_wall",
                    battery_powered = false,
                    security = "no",
                    items = {
                        { channel_id = 2, cc_id = class_ids.switch_binary, name = item_names.switch }
                    },
                    subdevices = {}
                }
            }
        }
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
    loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

    fixture:remove_node(node)

    loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
end

test_impl("Single Switch 2", {
    manufacturer_id = 271,
    product_type = 1027,
    product_id = 4096
})

test_impl("Single Switch 2", {
    manufacturer_id = 271,
    product_type = 1027,
    product_id = 8192
})

test_impl("Double Switch 2", {
    manufacturer_id = 271,
    product_type = 515,
    product_id = 4096
})

test_impl("Double Switch 2", {
    manufacturer_id = 271,
    product_type = 515,
    product_id = 8192
})
