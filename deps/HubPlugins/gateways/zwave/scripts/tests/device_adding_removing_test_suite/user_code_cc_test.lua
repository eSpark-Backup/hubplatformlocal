local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local source_device_info = {}

tests.override_function(_G, "loadfile", function(orig_loadfile, path)
    if path == "HUB:zwave/scripts/model/devices/devices_info/generated/0_1_2" then
        return function()
            local device_info = orig_loadfile("HUB:zwave/scripts/model/devices/devices_info/base_info")()

            for k, v in pairs(source_device_info) do
                device_info[k] = v
            end

            return device_info
        end
    else
        return orig_loadfile(path)
    end
end)

local function test_impl(params)

    source_device_info = params.source_device_info

    local cc_user_code = {
        id = class_ids.user_codes,
        supported_users_count = 250,
        version = 1
    }

    local channel_0 = {
        id = 0,
        classes = { cc_user_code }
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
            name = "user_codes",
            cc_id = class_ids.user_codes,
            channel_id = 0,
            value_type = "dictionary." .. item_value_type.usercode,
            value = {},
            elements_max_number = params.elements_max_number,
            user_code_modes = params.user_code_modes,
            user_code_restriction = params.user_code_restriction,
            force_checking_fields = { "elements_max_number_2", "user_code_modes", "user_code_restriction" }
        },
        {
            name = "user_codes_scan_progress",
            cc_id = class_ids.user_codes,
            channel_id = 0,
            value_type = item_value_type.int,
            value = 0,
        }
    }

    if not params.source_device_info or not params.source_device_info.ignore_user_code_modes then
        table.insert(
            expected_items,
            {
                name = "daily_user_code_intervals",
                cc_id = class_ids.user_codes,
                channel_id = 0,
                value_type = "dictionary.array.daily_interval",
                value = {},
                elements_max_number_2 = 30,
                force_checking_fields = { "elements_max_number" }
            })

        table.insert(
            expected_items,
            {
                name = "weekly_user_code_intervals",
                cc_id = class_ids.user_codes,
                channel_id = 0,
                value_type = "dictionary.array.weekly_interval",
                value = {},
                elements_max_number_2 = 30,
                force_checking_fields = { "elements_max_number" }
            })
    end

    local expected_devices = {
        {
            name = "Unknown",
            category = "unknown",
            items = expected_items,
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

    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )


    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = 0,
        product_type = 1,
        product_id = 2,
    } )

    -- check removing result
    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )

end

tests.run_with_params( test_impl,
{
    {
        value = {
            source_device_info = {},
            user_code_modes = { "enabled", "disabled" },
            user_code_restriction = "^\\d{4,10}$",
            elements_max_number = 250
        },
        description = "default device info",
    },
    {
        value = {
            source_device_info = {
                user_code_restriction = "^\\d{6,8}$",
                ignore_user_code_modes = true,
                first_user_code = 1,
                end_user_code = 100
            },
            user_code_restriction = "^\\d{6,8}$",
            elements_max_number = 100
        },
        description = "modified device info"
    }
} )