local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
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
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    firmware_versions = {
                        "2.102"
                    },
                    hardware_version = 48,
                    id = 134,
                    library_type = "routing_slave",
                    version = 2,
                    zwave_protocol_version = "4.24"
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
                            }
                        }
                    },
                    association_groups_count = 3,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 135,
                    version = 1
                },
                {
                    id = 128,
                    version = 1
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_GENERIC_SIREN",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_smart_include = false,
    manufacturer_id = 600,
    product_id = 136,
    product_type = 3,
    security = {
        "none"
    },
    security_class_started = {
        "none"
    },
    specific_type = "SPECIFIC_TYPE_SIREN"
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- check adding result
local expected_devices = {
    {
        name = "Siren",
        category = "siren",
        subcategory = "",
        items = { 
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            },
            {
                name = "battery",
                cc_id = class_ids.battery,
                channel_id = 0
            },
            {
                name = "siren_alarm",
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "siren"
            }
        },
        node_id = "9000",
        battery_powered = true,
        security = "no",
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 600,
    product_id = 136,
    product_type = 3,
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
