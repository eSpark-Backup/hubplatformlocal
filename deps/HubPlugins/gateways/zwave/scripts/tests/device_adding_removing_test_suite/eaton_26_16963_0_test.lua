local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local tests = require( "tests" )

zwave.request_node_values = tests.create_mock()

local node = {
    basic_type = 2,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = true,
                    version = 1
                },
                {
                    id = 112,
                    parameters = {},
                    version = 1
                },
                {
                    id = 114,
                    version = 1
                },
                {
                    firmware_versions = {
                        "3.1"
                    },
                    hardware_version = 0,
                    id = 134,
                    library_type = "static_controller",
                    version = 1,
                    zwave_protocol_version = "3.67"
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            }
                        }
                    },
                    association_groups_count = 1,
                    id = 133,
                    version = 1
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_GENERIC_CONTROLLER",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_smart_include = false,
    manufacturer_id = 26,
    product_id = 0,
    product_type = 16963,
    security = {
        "none"
    },
    security_class_started = {
        "none"
    },
    specific_type = "SPECIFIC_TYPE_NOT_USED"
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "Accessory Dimmer",
        category = "dimmable_light",
        subcategory = "dimmable_in_wall",
        items = {
            {
                name = "dimmer",
                cc_id = class_ids.basic,
                channel_id = 0,
 --               is_merged = false,
            },
        },
        node_id = "9000",
        battery_powered = false,
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

