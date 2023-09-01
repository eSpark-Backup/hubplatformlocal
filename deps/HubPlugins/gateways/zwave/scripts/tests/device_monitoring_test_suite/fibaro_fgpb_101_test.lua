local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local core = require("core")

zwave.request_node_values = tests.create_mock()

core.update_item_value = tests.create_mock()

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 0
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
                        "3.2"
                    },
                    hardware_version = 1,
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
                        }
                    },
                    id = 91,
                    version = 3
                },
                {
                    association_groups = {
                        {
                            commands = {
                                {
                                    class_id = 90,
                                    command_id = 1,
                                    name = "DEVICE_RESET_LOCALLY_NOTIFICATION"
                                },
                                {
                                    class_id = 128,
                                    command_id = 3,
                                    name = "BATTERY_REPORT"
                                },
                                {
                                    class_id = 91,
                                    command_id = 3,
                                    name = "CENTRAL_SCENE_NOTIFICATION"
                                },
                                {
                                    class_id = 113,
                                    command_id = 5,
                                    name = "NOTIFICATION_REPORT"
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
                            group_name = "On/Off",
                            profile = {
                                name = "Control:Key01",
                                profile_lsb = 1,
                                profile_msb = 32
                            }
                        },
                        {
                            commands = {
                                {
                                    class_id = 38,
                                    command_id = 1,
                                    name = "SWITCH_MULTILEVEL_SET"
                                },
                                {
                                    class_id = 38,
                                    command_id = 4,
                                    name = "SWITCH_MULTILEVEL_START_LEVEL_CHANGE"
                                },
                                {
                                    class_id = 38,
                                    command_id = 5,
                                    name = "SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE"
                                }
                            },
                            group_id = 3,
                            group_name = "Dimmer",
                            profile = {
                                name = "Control:Key01",
                                profile_lsb = 1,
                                profile_msb = 32
                            }
                        },
                        {
                            commands = {
                                {
                                    class_id = 156,
                                    command_id = 2,
                                    name = "SENSOR_ALARM_REPORT"
                                }
                            },
                            group_id = 4,
                            group_name = "Alarm",
                            profile = {
                                name = "Control:Key01",
                                profile_lsb = 1,
                                profile_msb = 32
                            }
                        }
                    },
                    association_groups_count = 4,
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 132,
                    max_wake_up_interval = 64800,
                    min_wake_up_interval = 0,
                    step_wake_up_interval = 3600,
                    version = 2,
                    wake_up_interval = 3600
                },
                {
                    id = 128,
                    version = 1
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "intrusion"
                            },
                            type = "home_security",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 5
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
                            group_nodes = {
                                {
                                    endpoint_id = 255,
                                    node_id = 1
                                }
                            },
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 4,
                    id = 142,
                    version = 3
                },
                {
                    firmware_ids = {
                        3873
                    },
                    id = 122,
                    version = 3
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 156,
                    sensor_types = {
                        "general"
                    },
                    version = 1
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_WALL_CONTROLLER",
    icon_type = "ICON_TYPE_GENERIC_WALL_CONTROLLER",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 271,
    product_id = 8192,
    product_type = 3841,
    security = {
        "s0"
    },
    security_class_started = {
        "s0"
    },
    specific_type = "SPECIFIC_TYPE_BASIC_WALL_CONTROLLER",
    wake_up_interval = 3600
}

fixture:add_node(node)

local button_item_id = storage:call("find_item_id", item_names.button_state, 9000, class_ids.central_scene, 0, "1")
assert(button_item_id)


-- test no crash when sensor alarm received
events_handler( {
    event = "alarm_sensor_value_updated",
    node_id = 9000,
    class_id = 156,
    channel_id = 0,
    sensor_type = "general",
    value = 255,
    delay = 0
} )

-- button pressed 1 time
events_handler( {
    event = "button_state_updated",
    node_id = 9000,
    class_id = class_ids.central_scene,
    channel_id = 0,
    button_id = 1,
    value = "press_1_time"
} )

tests.assert_called(
    core.update_item_value,
    "check button updated",
    { { button_item_id, { button_state = "press_1_time", button_number = 1 } }, { button_item_id, { button_state = "idle", button_number = 1 } } }
)

-- button held down
events_handler( {
    event = "button_state_updated",
    node_id = 9000,
    class_id = class_ids.central_scene,
    channel_id = 0,
    button_id = 1,
    value = "held_down"
} )

tests.assert_called(
    core.update_item_value,
    "check button updated",
    { { button_item_id, { button_state = "held_down", button_number = 1 } } }
)

-- button released
events_handler( {
    event = "button_state_updated",
    node_id = 9000,
    class_id = class_ids.central_scene,
    channel_id = 0,
    button_id = 1,
    value = "released"
} )

tests.assert_called(
    core.update_item_value,
    "check button updated",
    { { button_item_id, { button_state = "released", button_number = 1 } }, { button_item_id, { button_state = "idle", button_number = 1 } } }
)
