local tests = require( "tests" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
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
                        "2.4"
                    },
                    hardware_version = 5,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.34"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "power_has_been_applied"
                            },
                            type = "power_management",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 2
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
                    id = 37,
                    version = 1
                },
                {
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "float"
                            },
                            sensor_type = "general_purpose"
                        }
                    },
                    version = 1
                },
                {
                    id = 96,
                    version = 4
                },
                {
                    id = 152,
                    version = 0
                },
                {
                    firmware_ids = {
                        516
                    },
                    id = 122,
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
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "float"
                            },
                            sensor_type = "general_purpose"
                        }
                    },
                    version = 1
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
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "float"
                            },
                            sensor_type = "general_purpose"
                        }
                    },
                    version = 1
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
                    id = 37,
                    version = 1
                },
               {
                    id = 89,
                    version = 1
                }
            },
            id = 3
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
                    id = 89,
                    version = 1
                }
            },
            id = 4
        }
    },
    generic_type = "GENERIC_TYPE_SENSOR_MULTILEVEL",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 132,
    product_id = 516,
    product_type = 1123,
    security = {
        "s0"
    },
    security_class_started = {
        "s0"
    },
    specific_type = "SPECIFIC_TYPE_NOT_USED"
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
        name = "MIMO2+",
        category = "generic_sensor",
        items = { 
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0,
            },
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 3,
                is_merged = true
            },
            {
                name = "power_state",
                tag = "power_management",
                cc_id = class_ids.notification,
                channel_id = 0,
            },
        },
       node_id = "9000",
       subdevices = {
            {
                name = "Relative Modulation Level Sensor 2",
                category = "level_sensor",
                subcategory = "modulation",
                items = {
                    {
                        name = "relative_modulation_level",
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 1,
                        is_merged = true
                    },
                    {
                        name = "relative_modulation_level",
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0,

                    },
                },
            },
            {
                name = "Relative Modulation Level Sensor 1",
                category = "level_sensor",
                subcategory = "modulation",
                items = {
                    {
                        name = "relative_modulation_level",
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 2,
                    }
                },
            },
            {
                name = "MIMO2+ 1",
                category = "generic_sensor",
                subcategory = "",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 4,
                    }
                },
            },
        },
    },
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )--]]

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 99,
    product_id = 12856,
    product_type = 18770
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
