local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local core = require("core")
local tests = require( "tests" )


zwave.request_node_values = tests.create_mock()

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
                    version = 0
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 114,
                    version = 2
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
                                        "dw_is_opened",
                                        "dw_is_closed"
                                },
                                type = "access_control",
                                unknown_event_types = {}
                        },
                        {
                                event_types = {
                                        "product_cover_removed"
                                },
                                type = "home_security",
                                unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 4
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            },
                            group_nodes_max_count = 1
                        },
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
                                            class_id = 128,
                                            command_id = 3,
                                            name = "BATTERY_REPORT"
                                    },
                                    {
                                            class_id = 113,
                                            command_id = 5,
                                            name = "NOTIFICATION_REPORT"
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
                        }
                    },
                    association_groups_count = 1,
                    id = 89,
                    version = 1
                },
                {
                    id = 132,
                    max_wake_up_interval = 129600,
                    min_wake_up_interval = 1800,
                    step_wake_up_interval = 1800,
                    version = 2,
                    wake_up_interval = 5400
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
    icon_type = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_DOOR_WINDOW",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 316,
    product_id = 54,
    product_type = 2,
    security = {
        "s0"
    },
    security_class_started = {
        "s0"
    },
    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",
    wake_up_interval = 5400
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "Door/Window Sensor",
        category = "security_sensor",
        subcategory = "door",
        items = {
            {
                name = item_names.battery,
                cc_id = class_ids.battery,
                channel_id = 0
            },
            {
                name = item_names.tampering_cover_alarm,
                tag = item_descriptor_tags.home_security,
                cc_id = class_ids.notification,
                channel_id = 0,
            },
            {
                name = item_names.dw_state,
                tag = item_descriptor_tags.access_control,
                cc_id = class_ids.notification,
                channel_id = 0,
            },
            {
                name = item_names.security_threat,
                tag = "access_control.security_tag",
                cc_id = class_ids.notification,
                channel_id = 0,
                main_item_name = item_names.dw_state
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
                cc_id = class_ids.configuration,
                value_type = "dictionary.zwave_configuration",
                value = {}
            },
            {
                value_type = "int",
                value = 5400,
                value_default = 5400,
                cc_id = class_ids.wake_up,
                tag = "wake_up_interval",
                value_min = 1800,
                value_max = 129600,
                value_step = 1800,
            },
            {
                has_setter = false,
                value = {
                    ["0_1"] = {
                        channel = 0,
                        commands = {
                            "BATTERY_REPORT",
                            "NOTIFICATION_REPORT",
                            "DEVICE_RESET_LOCALLY_NOTIFICATION"
                        },
                        maxAssociationsNumber = 1,
                        name = "Lifeline",
                        profile = "General:Lifeline"
                    },
                },
                value_type = "dictionary.association_group",
                tag = "associations_groups"
            },
            {
                value = {},
                value_type = "dictionary.array.association",
                tag = "associations"
            }
        },--]]
        node_id = "9000",
        battery_powered = true,
        security = "low",
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
