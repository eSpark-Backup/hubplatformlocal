local fixture = require("HUB:zwave/scripts/tests/fixtures/base_fixture")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local zwave = require("zwave")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local function test_impl(params)
    local node = {
        id = "9000",
        manufacturer_id = 134,
        product_type = params.product_type,
        product_id = 100,
        is_battery_powered = false,

        generic_type = "GENERIC_TYPE_SENSOR_MULTILEVEL",
        specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL",
        icon_type = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_MOTION_ALARM",

        security = { "none" },
        security_class_started = { "none" },

        channels = {
            {
                id = 0,
                classes = {
                    {
                        id = class_ids.notification,
                        notifications = {
                            {
                                event_types = {
                                    "product_cover_removed"
                                },
                                type = "home_security",
                                unknown_event_types = {
                                    8
                                }
                            }
                        },
                        v1_supported = false,
                        version = 3
                    }
                }
            }
        }
    }

    fixture:add_node(node)

    local expected_devices = {
        {
            name = "Multisensor 6 in 1",
            category = "security_sensor",
            subcategory = "motion",
            items = {
                {
                    name = "motion",
                    cc_id = class_ids.notification,
                    tag = "home_security",
                    channel_id = 0,
                },
                {
                    name = "tampering_cover_alarm",
                    cc_id = class_ids.notification,
                    tag = "home_security",
                    channel_id = 0,
                },
                {
                    name = "security_threat",
                    tag = "home_security.security_tag",
                    cc_id = class_ids.notification,
                    channel_id = 0,
                    main_item_name = "motion"
                }
            },
            node_id = "9000",
            battery_powered = false
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
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                product_type = 258
            },
            description = "test with product_type = 258"
        },
        {
            value = {
                product_type = 2
            },
            description = "test with product_type = 2"
        }
    }
)
