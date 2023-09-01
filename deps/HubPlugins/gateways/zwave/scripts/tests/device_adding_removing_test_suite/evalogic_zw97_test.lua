local fixture = require("HUB:zwave/scripts/tests/fixtures/base_fixture")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local zwave = require("zwave")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
                },
                {
                    id = 112,
                    parameters = {},
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                {
                                    endpoint_id = 0,
                                    node_id = 1
                                }
                            }
                        }
                    },
                    association_groups_count = 3,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    application_build_number = 52445,
                    application_framework_api_build_number = 52445,
                    application_framework_api_version = "3.1.1",
                    application_version = "3.1.1",
                    firmware_versions = {
                        "1.1"
                    },
                    hardware_version = 2,
                    host_interface_build_number = 0,
                    id = 134,
                    library_type = "enhanced_slave",
                    protocol_build_number = 43,
                    protocol_version = "5.3.0",
                    sdk_version = "6.71.3",
                    version = 3,
                    zwave_protocol_version = "5.3"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 159,
                    version = 1
                },
                {
                    id = 96,
                    version = 4
                },
                {
                    firmware_ids = {
                        49159
                    },
                    id = 122,
                    version = 4
                }
            },
            id = 0
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 1
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 2
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 786,
    product_id = 49159,
    product_type = 49152,
    security = {
        "unauthenticated"
    },
    security_class_started = {
        "unauthenticated"
    },
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "Outdoor Smart Plug",
        category = "switch",
        subcategory = "exterior_plugin",
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            },
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 1,
                is_merged = true
            }
        },
        node_id = "9000",
        battery_powered = false,

        subdevices = {
            {
                name = "Outdoor Smart Plug 1",
                category = "switch",
                subcategory = "exterior_plugin",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 2
                    }
                },
            }
        }
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
