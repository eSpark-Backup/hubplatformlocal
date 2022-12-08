local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local function test_impl(product_info)
    local channel_0 = {
        id = 0,
        classes = {
            { id = 0x5E },
            { id = 0x72 },
            { id = 0x2B },
            { id = 0x2C },
            -- { id = class_ids.configuration },
            { id = 0x86 },
            { id = 0x60 },
            { id = 0x8E },
            { id = 0x85 },
            { id = 0x89 },
            { id = class_ids.switch_binary },
            {
                id = class_ids.notification,
                notifications = {
                    {
                        type = "home_security",
                        event_types = { "motion_detected" }
                    }
                }
            },
        }
    }

    local channel_1 = {
        id = 1,
        classes = {
            { id = 0x5E },
            { id = 0x59 },
            { id = class_ids.switch_binary },
        }
    }

    local channel_2 = {
        id = 2,
        classes = {
            { id = 0x5E },
            { id = 0x59 },
            {
                id = class_ids.notification,
                notifications = {
                    {
                        type = "home_security",
                        event_types = { "motion_detected" }
                    }
                }
            },
        }
    }

    local node = {
        id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id,
        is_battery_powered = false,

        generic_type = "GENERIC_TYPE_SWITCH_BINARY",
        specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
        icon_type = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0, channel_1, channel_2 }
    }

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )

    local expected_switch_items = {
        {
            name = "switch",
            cc_id = class_ids.switch_binary,
            channel_id = 0
        },
        {
            name = "switch",
            cc_id = class_ids.switch_binary,
            channel_id = 1,
            is_merged = true,
        },
    }

    local expected_motion_items = {
        {
            name = "motion",
            cc_id = class_ids.notification,
            tag = "home_security",
            channel_id = 0,
        },
        {
            name = "motion",
            cc_id = class_ids.notification,
            tag = "home_security",
            channel_id = 2,
            is_merged = true,
        },
        {
            name = "security_threat",
            tag = "home_security.security_tag",
            cc_id = class_ids.notification,
            channel_id = 0,
            main_item_name = "motion"
        }
    }

    local expected_devices = {
        {
            name = "In Wall Switch",
            category = "switch",
            subcategory = "in_wall",
            items = expected_switch_items,
            node_id = "9000",
            battery_powered = false,

            subdevices = {
                {
                    name = "Motion Sensor",
                    category = "security_sensor",
                    subcategory = "motion",
                    items = expected_motion_items
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

    loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id
    } )

    -- check node removing result
    loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params(test_impl, {
    {
        value = {
            manufacturer_id = 99,
            product_type = 18765,
            product_id = 12337
        }
    },
    {
        value = {
            manufacturer_id = 99,
            product_type = 18765,
            product_id = 12338
        }
    },
})
