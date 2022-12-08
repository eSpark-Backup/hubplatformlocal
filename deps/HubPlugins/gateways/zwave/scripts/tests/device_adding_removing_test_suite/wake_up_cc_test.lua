local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local function test_impl(params)
    local channel_0 = {
        id = 0,
        classes = {
            {
                id = class_ids.wake_up,
                version = 2,
                min_wake_up_interval = params.min,
                max_wake_up_interval = params.max,
                step_wake_up_interval = params.step,
                wake_up_interval = params.interval
            },
            {
                id = class_ids.switch_binary,
            }
        }
    }

    local node = {
        id = "9000",
        manufacturer_id = 0,
        product_type = 1,
        product_id = 2,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0 }
    }

    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )


    -- check adding result
    local expected_items = {
        {
            name = item_names.switch,
            cc_id = class_ids.switch_binary,
            channel_id = 0
        }
    }

    local expected_settings = {
        {
            value_type = "action",
            value = {
                text = "Rediscover",
                lang_tag = "rediscover_tag"
            },
            tag = "rediscover"
        }
    }

    if params.has_setting then
        table.insert(
            expected_settings,
            {
                value_type = "int",
                value = params.setting_value,
                value_default = params.setting_value,
                cc_id = class_ids.wake_up,
                tag = "wake_up_interval",
                value_min = params.min,
                value_max = params.max,
                value_step = params.step,
            }
        )
    end

    local expected_devices = {
        {
            name = "Plug",
            category = "switch",
            subcategory = "interior_plugin",
            items = expected_items,
            settings = expected_settings,
            node_id = "9000",
            battery_powered = true
        }
    }

    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {},
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

    loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )


    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = 0,
        product_type = 1,
        product_id = 2,
    } )

    -- check removing result
    loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params( test_impl,
{
    {
        value = {
            has_setting = true,
            setting_value = 5400
        },
        description = "no data in cc"
    },
    {
        value = {
            has_setting = true,
            interval = 3000,
            setting_value = 3000
        },
        description = "no capabilities"
    },
    {
        value = {
            has_setting = false,
            min = 0,
            max = 0
        },
        description = "0 capabilities"
    },
    {
        value = {
            has_setting = true,
            setting_value = 5400,
            min = 300,
            max = 64000,
            step = 60
        },
        description = "with capabilities"
    }
} )
