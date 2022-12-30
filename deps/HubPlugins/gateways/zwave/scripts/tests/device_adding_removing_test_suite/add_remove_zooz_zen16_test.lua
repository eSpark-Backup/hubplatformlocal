local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

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
                            },
                            group_nodes_max_count = 1
                        },
                        {
                            group_id = 2,
                            group_nodes_max_count = 5
                        },
                        {
                            group_id = 3,
                            group_nodes_max_count = 5
                        },
                        {
                            group_id = 4,
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 4,
                    id = 142,
                    version = 3
                },
                {
                    association_groups = {
                        {
                            commands = {
                                {
                                    class_id = 32,
                                    command_id = 3,
                                    name = "BASIC_REPORT"
                                },
                                {
                                    class_id = 37,
                                    command_id = 3,
                                    name = "SWITCH_BINARY_REPORT"
                                },
                                {
                                    class_id = 90,
                                    command_id = 1,
                                    name = "DEVICE_RESET_LOCALLY_NOTIFICATION"
                                }
                            },
                            group_id = 1,
                            group_name = "Lifeline",
                            profile = {
                                name = "General:Lifeline",
                                profile_lsb = 1,
                                profile_msb = 0
                            }
                        },
                        {
                            commands = {
                                {
                                    class_id = 32,
                                    command_id = 1,
                                    name = "BASIC_SET"
                                }
                            },
                            group_id = 2,
                            group_name = "BASIC SET EP 1",
                            profile = {
                                name = "Control:Key01",
                                profile_lsb = 1,
                                profile_msb = 32
                            }
                        },
                        {
                            commands = {
                                {
                                    class_id = 32,
                                    command_id = 1,
                                    name = "BASIC_SET"
                                }
                            },
                            group_id = 3,
                            group_name = "BASIC SET EP 2",
                            profile = {
                                name = "Control:Key02",
                                profile_lsb = 2,
                                profile_msb = 32
                            }
                        },
                        {
                            commands = {
                                {
                                    class_id = 32,
                                    command_id = 1,
                                    name = "BASIC_SET"
                                }
                            },
                            group_id = 4,
                            group_name = "BASIC SET EP 3",
                            profile = {
                                name = "Control:Key03",
                                profile_lsb = 3,
                                profile_msb = 32
                            }
                        }
                    },
                    association_groups_count = 4,
                    id = 89,
                    version = 1
                },
                {
                    application_build_number = 52445,
                    application_framework_api_build_number = 52445,
                    application_framework_api_version = "3.1.1",
                    application_version = "3.1.1",
                    firmware_versions = {
                        "1.4"
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
                    id = 96,
                    version = 4
                },
                {
                    id = 159,
                    version = 1
                },
                {
                    firmware_ids = {
                        40970
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
                    id = 89,
                    version = 1
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes_max_count = 0
                        },
                        {
                            group_id = 2,
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 2,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
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
                    is_supported = false,
                    version = 2
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
                    id = 89,
                    version = 1
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes_max_count = 0
                        },
                        {
                            group_id = 2,
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 2,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 2
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
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
                    id = 89,
                    version = 1
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes_max_count = 0
                        },
                        {
                            group_id = 2,
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 2,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 3
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 634,
    product_id = 40970,
    product_type = 40960,
    security = {
        "none"
    },
    security_class_started = {
        "unauthenticated"
    },
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local expected_devices = {
    {
        name = "Zooz Multirelay",
        category = "switch",
        subcategory = "in_wall",
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0,
            }
        },
        settings = {
            {
                value_type = "action",
                value = {
                    text = "Rediscover",
                    lang_tag = "rediscover_tag"
                },
                tag = "rediscover"
            },
            {
                value_type = "dictionary.association_group",
                tag = "associations_groups"
            },
            {
                value_type = "dictionary.array.association",
                tag = "associations"
            },
            {
                cc_id = class_ids.configuration,
                value_type = "dictionary.zwave_configuration",
                value = {}
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "restore",
                value = "restore",
                tag = "1",
                info = {
                    ["zwave.parameter_number"] = 1,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "toggle_switch",
                value = "toggle_switch",
                tag = "2",
                info = {
                    ["zwave.parameter_number"] = 2,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "toggle_switch",
                value = "toggle_switch",
                tag = "3",
                info = {
                    ["zwave.parameter_number"] = 3,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "toggle_switch",
                value = "toggle_switch",
                tag = "4",
                info = {
                    ["zwave.parameter_number"] = 4,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "led_on_when_all_off",
                value = "led_on_when_all_off",
                tag = "5",
                info = {
                    ["zwave.parameter_number"] = 5,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "int",
                value_default = 0,
                value_min = 0,
                value_max = 65535,
                value = 0,
                tag = "6",
                info = {
                    ["zwave.parameter_number"] = 6,
                    ["zwave.parameter_size"] = 4
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "int",
                value_default = 0,
                value_min = 0,
                value_max = 65535,
                value = 0,
                tag = "7",
                info = {
                    ["zwave.parameter_number"] = 7,
                    ["zwave.parameter_size"] = 4
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "int",
                value_default = 0,
                value_min = 0,
                value_max = 65535,
                value = 0,
                tag = "8",
                info = {
                    ["zwave.parameter_number"] = 8,
                    ["zwave.parameter_size"] = 4
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "int",
                value_default = 0,
                value_min = 0,
                value_max = 65535,
                value = 0,
                tag = "9",
                info = {
                    ["zwave.parameter_number"] = 9,
                    ["zwave.parameter_size"] = 4
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "int",
                value_default = 0,
                value_min = 0,
                value_max = 65535,
                value = 0,
                tag = "10",
                info = {
                    ["zwave.parameter_number"] = 10,
                    ["zwave.parameter_size"] = 4
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "int",
                value_default = 0,
                value_min = 0,
                value_max = 65535,
                value = 0,
                tag = "11",
                info = {
                    ["zwave.parameter_number"] = 11,
                    ["zwave.parameter_size"] = 4
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "enabled",
                value = "enabled",
                tag = "12",
                info = {
                    ["zwave.parameter_number"] = 12,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "enabled",
                value = "enabled",
                tag = "13",
                info = {
                    ["zwave.parameter_number"] = 13,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "enabled",
                value = "enabled",
                tag = "14",
                info = {
                    ["zwave.parameter_number"] = 14,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "minutes",
                value = "minutes",
                tag = "15",
                info = {
                    ["zwave.parameter_number"] = 15,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "minutes",
                value = "minutes",
                tag = "16",
                info = {
                    ["zwave.parameter_number"] = 16,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "minutes",
                value = "minutes",
                tag = "17",
                info = {
                    ["zwave.parameter_number"] = 17,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "minutes",
                value = "minutes",
                tag = "18",
                info = {
                    ["zwave.parameter_number"] = 18,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "minutes",
                value = "minutes",
                tag = "19",
                info = {
                    ["zwave.parameter_number"] = 19,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "minutes",
                value = "minutes",
                tag = "20",
                info = {
                    ["zwave.parameter_number"] = 20,
                    ["zwave.parameter_size"] = 1
                }
            },
            {
                cc_id = class_ids.configuration,
                value_type = "token",
                ignore_enum = true,
                value_default = "disabled",
                value = "disabled",
                tag = "24",
                info = {
                    ["zwave.parameter_number"] = 24,
                    ["zwave.parameter_size"] = 1
                }
            },
        },
        node_id = "9000",
        battery_powered = false,

        subdevices = {
            {
                name = "Multirelay Switch 1",
                category = "switch",
                subcategory = "in_wall",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 1
                    }
                },
            },
            {
                name = "Multirelay Switch 2",
                category = "switch",
                subcategory = "in_wall",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 2
                    }
                },
            },
            {
                name = "Multirelay Switch 3",
                category = "switch",
                subcategory = "in_wall",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 3
                    }
                },
            }
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 634,
    product_id = 40968,
    product_type = 40960
} )

-- check node removing result
hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
