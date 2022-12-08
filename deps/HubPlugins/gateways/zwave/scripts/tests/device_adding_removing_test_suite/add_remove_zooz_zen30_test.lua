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
                    id = 38,
                    version = 4
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {},
                            group_nodes_max_count = 1
                        },
                        {
                            group_id = 2,
                            group_nodes_max_count = 5
                        },
                        {
                            group_id = 3,
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 3,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
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
                                    class_id = 91,
                                    command_id = 3,
                                    name = "CENTRAL_SCENE_NOTIFICATION"
                                },
                                {
                                    class_id = 38,
                                    command_id = 3,
                                    name = "SWITCH_MULTILEVEL_REPORT"
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
                            group_name = "BASIC SET EP 0",
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
                            group_name = "BASIC SET EP 1",
                            profile = {
                                name = "Control:Key02",
                                profile_lsb = 2,
                                profile_msb = 32
                            }
                        }
                    },
                    association_groups_count = 3,
                    id = 89,
                    version = 1
                },
                {
                    application_build_number = 52445,
                    application_framework_api_build_number = 52445,
                    application_framework_api_version = "3.1.1",
                    application_version = "3.1.1",
                    firmware_versions = {
                        "1.3"
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
                    buttons = {
                        {
                            button_attributes = {
                                "press_1_time",
                                "released",
                                "held_down",
                                "press_2_times",
                                "press_3_times",
                                "press_4_times",
                                "press_5_times"
                            },
                            button_id = 1
                        },
                        {
                            button_attributes = {
                                "press_1_time",
                                "released",
                                "held_down",
                                "press_2_times",
                                "press_3_times",
                                "press_4_times",
                                "press_5_times"
                            },
                            button_id = 2
                        },
                        {
                            button_attributes = {
                                "press_1_time",
                                "released",
                                "held_down",
                                "press_2_times",
                                "press_3_times",
                                "press_4_times",
                                "press_5_times"
                            },
                            button_id = 3
                        }
                    },
                    id = 91,
                    version = 3
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
                        40968
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
                    is_supported = true,
                    version = 2
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
        }
    },
    command_span = 0,
    generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
    icon_type = "ICON_TYPE_GENERIC_LIGHT_DIMMER_SWITCH",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 634,
    product_id = 40968,
    product_type = 40960,
    security = {
        "unauthenticated"
    },
    security_class_started = {
        "unauthenticated"
    },
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL"
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
        name = "Wall Dimmer Switch",
        category = "dimmable_light",
        subcategory = "dimmable_in_wall",
        items = {
            {
                name = "switch",
                cc_id = class_ids.dimmer,
                channel_id = 0,
            },            
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0,
                is_merged = true
            },
            {
                name = "dimmer",
                cc_id = class_ids.dimmer,
                channel_id = 0,
            },
            {
                name = "dimmer_up",
                cc_id = class_ids.dimmer,
                channel_id = 0,
            },
            {
                name = "dimmer_down",
                cc_id = class_ids.dimmer,
                channel_id = 0,
            },
            {
                name = "dimmer_stop",
                cc_id = class_ids.dimmer,
                channel_id = 0,
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {
                    "idle",
                    "press_1_time",
                    "released",
                    "held_down",
                    "press_2_times",
                    "press_3_times",
                    "press_4_times",
                    "press_5_times"
                },
                tag = "1",
                value = {button_number = 1, button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {
                    "idle",
                    "press_1_time",
                    "released",
                    "held_down",
                    "press_2_times",
                    "press_3_times",
                    "press_4_times",
                    "press_5_times"
                },
                tag = "2",
                value = {button_number = 2, button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {
                    "idle",
                    "press_1_time",
                    "released",
                    "held_down",
                    "press_2_times",
                    "press_3_times",
                    "press_4_times",
                    "press_5_times"
                },
                tag = "3",
                value = {button_number = 3, button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
        },
        node_id = "9000",
        battery_powered = false,
        
        subdevices = {
            {
                name = "Wall Second Switch",
                category = "switch",
                subcategory = "in_wall",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.basic,
                        channel_id = 1
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
