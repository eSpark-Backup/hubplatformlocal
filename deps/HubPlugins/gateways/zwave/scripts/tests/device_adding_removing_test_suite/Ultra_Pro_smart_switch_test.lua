local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local function test_impl(product_id)
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
                        parameters = {
                            {
                                default_value = 2,
                                format = "signed_integer",
                                id = 3,
                                info = "Set the LED light status",
                                max_value = 3,
                                min_value = 0,
                                name = "LED Light",
                                read_only = false,
                                size = 1
                            },
                            {
                                default_value = 0,
                                format = "signed_integer",
                                id = 4,
                                info = "Reverse the default On/Off rocker setting",
                                max_value = 1,
                                min_value = 0,
                                name = "Invert Switch",
                                read_only = false,
                                size = 1
                            },
                            {
                                default_value = 0,
                                format = "signed_integer",
                                id = 19,
                                info = "Alternative way to perform Exclusion",
                                max_value = 1,
                                min_value = 0,
                                name = "Alternate Exclusion",
                                read_only = false,
                                size = 1
                            }
                        },
                        version = 4
                    },
                    {
                        id = 94,
                        version = 2
                    },
                    {
                        id = 37,
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
                        application_build_number = 52445,
                        application_framework_api_build_number = 52445,
                        application_framework_api_version = "4.1.2",
                        application_version = "4.1.2",
                        firmware_versions = {
                            "5.53"
                        },
                        hardware_version = 255,
                        host_interface_build_number = 0,
                        id = 134,
                        library_type = "enhanced_slave",
                        protocol_build_number = 91,
                        protocol_version = "6.4.0",
                        sdk_version = "6.81.3",
                        version = 3,
                        zwave_protocol_version = "6.4"
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
                                    "press_3_times"
                                },
                                button_id = 1
                            },
                            {
                                button_attributes = {
                                    "press_1_time",
                                    "released",
                                    "held_down",
                                    "press_2_times",
                                    "press_3_times"
                                },
                                button_id = 2
                            }
                        },
                        id = 91,
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
                        firmware_ids = {
                            29240
                        },
                        id = 122,
                        version = 4
                    }
                },
                id = 0
            }
        },
        generic_type = "GENERIC_TYPE_SWITCH_BINARY",
        icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_WALL_LAMP",
        id = "9000",
        is_battery_powered = false,
        is_reachable = false,
        is_smart_include = false,
        manufacturer_id = 99,
        product_id = product_id,
        product_type = 18770,
        security = {
            "none"
        },
        security_class_started = {
            "authenticated"
        },
        specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
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
            name = "In Wall Switch",
            category = "switch",
            subcategory = "in_wall",
            items = {
                {
                    name = "switch",
                    cc_id = class_ids.switch_binary,
                    channel_id = 0
                },
                {
                    name = "button_state",
                    value_type = "button_state",
                    enum = {"press_1_time", "released", "held_down", "press_2_times", "press_3_times", "idle"},
                    tag = "1",
                    value = {button_number = 1, button_state = "idle"},
                    cc_id = class_ids.central_scene,
                    channel_id = 0
                },
                {
                    name = "button_state",
                    value_type = "button_state",
                    enum = {"press_1_time", "released", "held_down", "press_2_times", "press_3_times", "idle"},
                    tag = "2",
                    value = {button_number = 2, button_state = "idle"},
                    cc_id = class_ids.central_scene,
                    channel_id = 0
                }
            },
            node_id = "9000"
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
        manufacturer_id = 99,
        product_id = product_id,
        product_type = 18770
    } )

    -- check node removing result
    loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = 12855,
            description = "test with 12855 product_id"
        },
        {
            value = 12856,
            description = "test with 12856 product_id"
        }
    }
)
