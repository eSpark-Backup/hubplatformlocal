--TODO: Zipato Z-Wave Remote PH-PSR03.US is not integrated yet

local fixture = require("HUB:zwave/scripts/tests/fixtures/base_fixture")
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
                    id = 128,
                    version = 1
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "contact_medical_service"
                            },
                            type = "emergency_alarm",
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
                            }
                        }
                    },
                    association_groups_count = 9,
                    id = 133,
                    version = 2
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    firmware_versions = {
                        "1.18",
                        "1.9"
                    },
                    hardware_version = 1,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
                },
                {
                    id = 132,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    firmware_ids = {
                        2077
                    },
                    id = 122,
                    version = 2
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_GENERIC_CONTROLLER",
    icon_type = "ICON_TYPE_GENERIC_REMOTE_CONTROL_AV",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 316,
    product_id = 29,
    product_type = 8,
    security = {
        "none"
    },
    security_class_started = {
        "s0"
    },
    specific_type = "SPECIFIC_TYPE_REMOTE_CONTROL_AV",
    wake_up_interval = 5400
}

fixture:add_node(node)

local function generate_button_item(button_number)
    return {
        name = "button_state",
        value_type = "button_state",
        enum = {
            "idle",
            "press_1_time",
            "released",
            "held_down",
        },
        tag = tostring(button_number),
        value = {button_number = button_number, button_state = "idle"},
        cc_id = 148,
        channel_id = 0
    }
end

local expected_devices = {
    {
        name = "Panic Button",
        category = "scene_controller",
        subcategory = "panic",
        items = {
            {
                name = "battery",
                value = 100,
                channel_id = 0,
                cc_id = 0x80
            },
            generate_button_item(1),
            generate_button_item(2),
            generate_button_item(3),
            generate_button_item(4),
            generate_button_item(5),
            generate_button_item(6),
            generate_button_item(7),
            generate_button_item(8),
        },
        node_id = "9000",
        battery_powered = true
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
