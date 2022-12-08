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
                    version = 1
                },
                {
                    id = 112,
                    parameters = {},
                    version = 0
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    firmware_versions = {
                        "1.39"
                    },
                    hardware_version = 255,
                    id = 134,
                    library_type = "routing_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            },
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 1,
                    id = 133,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            commands = {
                                {
                                    class_id = 90,
                                    command_id = 1,
                                    name = "DEVICE_RESET_LOCALLY_NOTIFICATION"
                                }
                            },
                            group_id = 1,
                            group_name = "Lifeline",
                            profile = {
                                name = "General:NA",
                                profile_lsb = 0,
                                profile_msb = 0
                            }
                        }
                    },
                    association_groups_count = 1,
                    id = 89,
                    version = 1
                },
                {
                    id = 37,
                    version = 1
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 0,
    product_id = 2,
    product_type = 3,
    security = {
        "none"
    },
    security_class_started = {
        "none"
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

-- check adding result
local expected_switch_items = {
    {
        name = "switch",
        cc_id = class_ids.switch_binary,
        channel_id = 0,
    }
}

local expected_devices = {
    {
        name = "Plug",
        category = "switch",
        subcategory = "interior_plugin",
        items = expected_switch_items,
        node_id = "9000",
        battery_powered = false
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
    manufacturer_id = 0,
    product_type = 2,
    product_id = 3
} )

-- check node removing result
hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
