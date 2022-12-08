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
                        "4.0"
                    },
                    hardware_version = 2,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
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
                    id = 48,
                    sensor_types = {},
                    version = 1
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "overheat_detected"
                            },
                            type = "heat_alarm",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 5
                },
                {
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "celsius"
                            },
                            sensor_type = "air_temperature"
                        }
                    },
                    version = 7
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
                        },
                        {
                            group_id = 6,
                            group_nodes = {
                                {
                                    endpoint_id = 0,
                                    node_id = 1
                                }
                            }
                        }
                    },
                    association_groups_count = 6,
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
                    firmware_versions = {
                        "4.0"
                    },
                    hardware_version = 2,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
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
                    id = 94,
                    version = 2
                },
                {
                    firmware_versions = {
                        "4.0"
                    },
                    hardware_version = 2,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
                },
                {
                    id = 48,
                    sensor_types = {},
                    version = 1
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "overheat_detected"
                            },
                            type = "heat_alarm",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 5
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
                }
            },
            id = 2
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
                    firmware_versions = {
                        "4.0"
                    },
                    hardware_version = 2,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
                },
                {
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "celsius"
                            },
                            sensor_type = "air_temperature"
                        }
                    },
                    version = 7
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
                }
            },
            id = 3
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_WALL_OUTLET",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 345,
    product_id = 83,
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
        name = "Flush 1D Relay",
        category = "switch",
        subcategory = "in_wall",
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            }
        },
        node_id = "9000",
        battery_powered = false,

        subdevices = {
            {
                name = "Heat Sensor",
                category = "security_sensor",
                subcategory = "heat",
                items = {
                    {
                        name = "heat_alarm",
                        tag = "heat_alarm",
                        cc_id = class_ids.notification,
                        channel_id = 0,
                        value_type = "token",
                        enum = {
                            "unknown",
                            "heat_ok",
                            "overheat_detected",
                        }
                    },
                    {
                        name = "security_threat",
                        tag = "heat_alarm.security_tag",
                        cc_id = class_ids.notification,
                        channel_id = 0,
                        main_item_name = "heat_alarm",
                    }
                }
            },
            {
                name = "Temperature Sensor",
                category = "temperature",
                subcategory = "",
                items = {
                    {
                        name = "temp",
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
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
