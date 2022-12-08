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
                    version = 2
                },
                {
                    firmware_versions = {
                        "4.1",
                        "4.1"
                    },
                    hardware_version = 1,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.61"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
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
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "over_current_detected",
                                "over_load_detected"
                            },
                            type = "power_management",
                            unknown_event_types = {}
                        },
                        {
                            event_types = {
                                "system_hardware_failure"
                            },
                            type = "system",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 96,
                    version = 4
                },
                {
                    firmware_ids = {
                        5153,
                        5153
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
                    version = 1
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
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                },
                {
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "over_current_detected",
                                "over_load_detected"
                            },
                            type = "power_management",
                            unknown_event_types = {}
                        },
                        {
                            event_types = {
                                "system_hardware_failure"
                            },
                            type = "system",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
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
                    is_supported = true,
                    version = 1
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
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 3
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
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
    manufacturer_id = 271,
    product_id = 8192,
    product_type = 5121,
    security = {
        "authenticated"
    },
    security_class_started = {
        "authenticated"
    },
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "Wall Plug",
        category = "switch",
        subcategory = "interior_plugin",
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
            },
            {
                name = "meter_reset",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "meter_reset",
                cc_id = class_ids.meter,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "electric_meter_kwh",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "electric_meter_kwh",
                cc_id = class_ids.meter,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "electric_meter_watt",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "electric_meter_watt",
                cc_id = class_ids.meter,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "over_current_state",
                cc_id = class_ids.notification,
                tag = "power_management",
                channel_id = 0
            },
            {
                name = "over_current_state",
                cc_id = class_ids.notification,
                tag = "power_management",
                channel_id = 1,
                is_merged = true
            },
            {
                name = "over_load_state",
                tag = "power_management",
                cc_id = class_ids.notification,
                channel_id = 0
            },
            {
                name = "over_load_state",
                tag = "power_management",
                cc_id = class_ids.notification,
                channel_id = 1,
                is_merged = true
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
                name = "hw_state",
                cc_id = class_ids.notification,
                channel_id = 1,
                value_type = "token",
                enum = {"unknown", "hardware_ok", "hardware_failure"},
                value = "hardware_ok",
                tag = "system",
                is_merged = true
            }
        },
        node_id = "9000",
        battery_powered = false,

        subdevices = {
            {
                name = "Wall Plug 1",
                category = "power_meter",
                items = {
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
                }
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
