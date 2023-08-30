local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local function test_impl(param)

    local cc_user_code = {
        id = class_ids.user_codes,
        supported_users_count = 250,
        version = 2,
        capabilities = param.capabilities
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
            maxElementsCount = 250,
            user_code_modes = param.expected_user_code_modes,
            user_code_restriction = "^[0123456789abc]{4,10}$",
            force_checking_fields = { "maxElementsCount2", "user_code_modes", "string_restriction", "user_code_restriction" }
        },
        {
            name = "user_codes_scan_progress",
            cc_id = class_ids.user_codes,
            channel_id = 0,
            value_type = item_value_type.int,
            value = 0,
        },
    }

    if param.has_virtual_restrictions then
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

    for _, item in ipairs(param.additional_expected_items) do
        table.insert(expected_items, item)
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

    local check_context = {}

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


tests.run_with_params(
    test_impl,
    {
        {
            value = {
                capabilities = {
                    checksum_supported = true,
                    master_code_supported = false,
                    master_code_deactivation_supported = false,
                    supported_keys = "0123456789abc",
                    supported_keypad_modes = { },
                    supported_user_id_statuses = { }
                },
                additional_expected_items = { },
                expected_user_code_modes = { "enabled" },
                has_virtual_restrictions = false
            },
            description = "Minimum of capabilities"
        },
        {
            value = {
                capabilities = {
                    checksum_supported = true,
                    master_code_supported = true,
                    master_code_deactivation_supported = false,
                    supported_keys = "0123456789abc",
                    supported_keypad_modes = { },
                    supported_user_id_statuses = { "enabled", "disabled", "available", "messaging" }
                },
                additional_expected_items = {
                    {
                        name = "master_code",
                        cc_id = class_ids.user_codes,
                        channel_id = 0,
                        value_type = item_value_type.string,
                        value = "****",
                        string_restrinction = "^[0123456789abc]{4,10}$"
                    }
                },
                expected_user_code_modes = { "enabled", "disabled", "messaging" },
                has_virtual_restrictions = true
            },
            description = "Master code is enabled; two user code modes"
        },
        {
            value = {
                capabilities = {
                    checksum_supported = true,
                    master_code_supported = true,
                    master_code_deactivation_supported = true,
                    supported_keys = "0123456789abc",
                    supported_keypad_modes = { },
                    supported_user_id_statuses = { "enabled", "disabled", "passage" }
                },
                additional_expected_items = {
                    {
                        name = "master_code",
                        cc_id = class_ids.user_codes,
                        channel_id = 0,
                        value_type = item_value_type.string,
                        value = "****",
                    },
                    {
                        name = "master_code_state",
                        cc_id = class_ids.user_codes,
                        channel_id = 0,
                        value_type = item_value_type.token,
                        value = "unknown",
                        enum = {
                            "unknown",
                            "enabled",
                            "disabled"
                        }
                    }
                },
                expected_user_code_modes = { "enabled", "disabled", "passage" },
                has_virtual_restrictions = true
            },
            description = "Master code is enabled; Master code deactivation is enabled; three user code modes"
        },
        {
            value = {
                capabilities = {
                    checksum_supported = false,
                    master_code_supported = false,
                    master_code_deactivation_supported = false,
                    supported_keys = "0123456789abc",
                    supported_keypad_modes = { "normal", "vacation", "privacy" },
                    supported_user_id_statuses = { "enabled", "disabled", "passage" }
                },
                additional_expected_items = {
                    {
                        name = "user_codes_keypad_mode",
                        cc_id = class_ids.user_codes,
                        channel_id = 0,
                        value_type = item_value_type.token,
                        value = "unknown",
                        enum = {
                            "normal",
                            "vacation",
                            "privacy",
                            "unknown"
                        }
                    }
                },
                expected_user_code_modes = { "enabled", "disabled", "passage" },
                has_virtual_restrictions = true
            },
            description = "three keypad modes; three user code modes"
        },
    }
)
