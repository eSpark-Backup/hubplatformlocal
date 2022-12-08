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
                    id = 94,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 129,
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
                    version = 4
                },
                {
                    id = 96,
                    version = 4
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
                    association_groups_count = 4,
                    id = 142,
                    version = 3
                },
                {
                    id = 44,
                    version = 1
                },
                {
                    id = 43,
                    version = 1
                },
                {
                    id = 112,
                    parameters = {},
                    version = 1
                },
                {
                    firmware_versions = {
                        "2.0"
                    },
                    hardware_version = 140,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.54"
                },
                {
                    id = 114,
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
                    firmware_ids = {
                        140
                    },
                    id = 122,
                    version = 3
                }
            },
            id = 0
        },
        {
            classes = {
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 89,
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
                    id = 37,
                    version = 1
                }
            },
            id = 1
        },
        {
            classes = {
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 89,
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
                    id = 37,
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
    is_smart_include = false,
    manufacturer_id = 134,
    product_id = 140,
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
local expected_devices = {
    {
        name = "Dual Nano Switch",
        category = "switch",
        subcategory = "in_wall",
        items = { 
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0,
            },
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 1,
                is_merged = true
            }
        },
        
        subdevices = {
            {
                name = "Dual Nano Switch - 2nd Switch",
                category = "switch",
                subcategory = "in_wall",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 2,
                    },
                }
            },
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
            }
        },
        
        node_id = "9000",
        battery_powered = false,
        security = "authenticated",
    }
}

local check_context = {}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 134,
    product_id = 140,
    product_type = 3,
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
