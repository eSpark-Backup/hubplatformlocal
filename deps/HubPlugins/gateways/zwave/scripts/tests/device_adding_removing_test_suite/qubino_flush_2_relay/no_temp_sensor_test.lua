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
                    version = 1
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
                    firmware_versions = {
                        "5.1",
                        "5.1"
                    },
                    hardware_version = 2,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.38"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 4
                },
                {
                    id = 96,
                    version = 4
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
                    association_groups_count = 8,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                }
            },
            id = 0
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 4
                }
            },
            id = 1
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 4
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
    manufacturer_id = 345,
    product_id = 81,
    product_type = 2,
    security = {
        "none"
    },
    security_class_started = {
        "none"
    },
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "Flush 2 Relay",
        category = "switch",
        subcategory = "in_wall",
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            },
            {
                name = "meter_reset",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "electric_meter_kwh",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "electric_meter_watt",
                cc_id = class_ids.meter,
                channel_id = 0
            }
        },
        node_id = "9000",
        battery_powered = false,

        subdevices = {
            {
                name = "Flush 2 Relay 1",
                category = "switch",
                subcategory = "in_wall",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 1
                    },
                    {
                        name = "meter_reset",
                        cc_id = class_ids.meter,
                        channel_id = 1
                    },
                    {
                        name = "electric_meter_kwh",
                        cc_id = class_ids.meter,
                        channel_id = 1
                    },
                    {
                        name = "electric_meter_watt",
                        cc_id = class_ids.meter,
                        channel_id = 1
                    }
                },
            },
            {
                name = "Flush 2 Relay 2",
                category = "switch",
                subcategory = "in_wall",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 2
                    },
                    {
                        name = "meter_reset",
                        cc_id = class_ids.meter,
                        channel_id = 2
                    },
                    {
                        name = "electric_meter_kwh",
                        cc_id = class_ids.meter,
                        channel_id = 2
                    },
                    {
                        name = "electric_meter_watt",
                        cc_id = class_ids.meter,
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
