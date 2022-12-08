local fixture = require("HUB:zwave/scripts/tests/fixtures/base_fixture")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local zwave = require("zwave")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()
zwave.remove_node_from_association_group = tests.create_mock()
zwave.add_node_to_association_group = tests.create_mock()

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "system_software_failure",
                                "system_hardware_failure"
                            },
                            type = "system",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_GENERIC_ON_OFF_WALL_SWITCH",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 1,
    product_id = 2,
    product_type = 3,
    security = {
        "s0"
    },
    security_class_started = {
        "s0"
    },
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "In Wall Switch",
        category = "switch",
        subcategory = "in_wall",
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            },
            {
                name = "hw_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                value_type = "token",
                enum = {"unknown", "hardware_ok", "hardware_failure"},
                value = "hardware_ok",
                tag = "system"
            },
            {
                name = "sw_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                value_type = "token",
                enum = {"unknown", "software_ok", "software_failure"},
                value = "software_ok",
                tag = "system"
            }
        },
        node_id = "9000",
        battery_powered = false
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
