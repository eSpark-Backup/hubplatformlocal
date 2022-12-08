local fixture = {
    zwave_info = {
        module = "Z-Wave",
        module_firmware_version = "6.04",
        locale = "US",
        home_id = "123",
        node_id = 1,
        roles = {
            sis = true,
            primary = true,
            secondary = false,
            static = false,
            inclusion = false
        }
    }
}

local core = require("core")
local tests = require("tests")
local zwave = require "zwave"

function fixture.init()
    core.set_plugin_id("zwave")
   
    zwave.subscribe = tests.create_mock()
    core.subscribe = tests.create_mock()
   
    zwave.set_devices_poll_intervals = tests.create_mock()
    zwave.get_info = function()
        return fixture.zwave_info
    end
end

function fixture.check_gateway_settings(expected)
    local actual = core.get_gateway_setting_ids_by_gateway_id()
    tests.assert_eq( #actual, #expected, "check settings_count" )
    
    for _, expected_setting in ipairs(expected) do
        local actual_setting = core.get_gateway_setting("zwave." .. expected_setting.id)
        tests.assert_ne( actual_setting, nil, "check setting exists: " .. expected_setting.id )
        
        tests.assert_eq( actual_setting.label, expected_setting.label, "check label" )
        tests.assert_eq( actual_setting.description, expected_setting.description, "check description" )
        tests.assert_eq( actual_setting.status, expected_setting.status, "check status" )
        tests.assert_eq( actual_setting.value_type, expected_setting.value_type, "check value_type" )
        tests.assert_eq( actual_setting.value, expected_setting.value, "check value" )
        tests.assert_eq( actual_setting.value_min, expected_setting.value_min, "check value_min" )
        tests.assert_eq( actual_setting.value_max, expected_setting.value_max, "check value_max" )
        tests.assert_eq( actual_setting.value_default, expected_setting.value_default, "check value_default" )
        tests.assert_eq( actual_setting.has_setter, expected_setting.has_setter, "check has_setter" )
        if expected_setting.value_type == "token" then
            tests.assert_eq( actual_setting.enum, expected_setting.enum, "check enum" )
        end
    end
end

function fixture.set_zwave_info_field(key, value)
    fixture.zwave_info[key] = value
end

function fixture.get_mandatory_settings_expectation()
    return {
        {
            id = "mains_devices_polling_interval",
            label = {
                text = "Mains powered devices polling interval in seconds",
                lang_tag = "gateway_setting_mains_devices_polling_interval"
            },
            status = "synced",
            value_type = "int",
            value = 10800,
            value_default = 10800,
            has_setter = true
        },
        {
            id = "reachable_check_attempts_count",
            label = {
                text = "Device connection attempts count",
                lang_tag = "gateway_setting_reachable_check_attempts_count"
            },
            status = "synced",
            value_type = "int",
            value = 3,
            value_default = 3,
            has_setter = true
        },
        {
            id = "reachable_check_policy",
            label = {
                text = "Device reachable check timeouts policy",
                lang_tag = "gateway_setting_reachable_check_policy"
            },
            status = "synced",
            value_type = "token",
            enum = {
                default = { text = "Default", lang_tag = "gateway_setting_reachable_check_policy_default" },
                use_fixed_interval = { text = "Use only fixed interval", lang_tag = "gateway_setting_reachable_check_policy_use_fixed_interval" }        
            },
            value = "default",
            value_default = "default",
            has_setter = true
        },
        {
            id = "wake_up_devices_polling_interval",
            label = {
                text = "Wake up devices polling interval in seconds",
                lang_tag = "gateway_setting_wake_up_devices_polling_interval"
            },
            status = "synced",
            value_type = "int",
            value = 5400,
            value_default = 5400,
            has_setter = true
        },
        {
            id = "zwave_reconstruct",
            label = {
                text = "ZWave reconstruct",
                lang_tag = "gateway_setting_zwave_reconstruct"
            },
            description = {
               lang_tag = "gateway_setting_zwave_reconstruct_description",
               text = "This action allow you to reconstruct your current Z-Wave network if something went wrong."
            },
            status = "synced",
            value_type = "action",
            value = {
                lang_tag = "reconstruct_zwave_button",
                text = "Reconstruct Z-Wave network"
            },
            has_setter = true
        },
        {
            id = "zwave_reset",
            label = {
                text = "ZWave reset",
                lang_tag = "gateway_setting_zwave_reset"
            },
            description = {
               lang_tag = "gateway_setting_zwave_reset_description",
               text = "This action allow you to reset your current Z-Wave network if something went wrong."
            },
            status = "synced",
            value_type = "action",
            value = {
                lang_tag = "reset_zwave_button",
                text = "Reset Z-Wave network"
            },
            has_setter = true
        },
        -- info settings
        {
            id = "home_id",
            label = {
                text = "Home ID",
                lang_tag = "gateway_setting_home_id"
            },
            status = "synced",
            value_type = "string",
            value = "123",
            has_setter = false
        },
        {
            id = "locale",
            label = {
                text = "Locale",
                lang_tag = "gateway_setting_locale"
            },
            status = "synced",
            value_type = "string",
            value = "US",
            has_setter = false
        },
        {
            id = "node_id",
            label = {
                text = "Node ID",
                lang_tag = "gateway_setting_node_id"
            },
            status = "synced",
            value_type = "string",
            value = "1",
            has_setter = false
        },
        {
            id = "role",
            label = {
                text = "Role",
                lang_tag = "gateway_setting_role"
            },
            status = "synced",
            value_type = "string",
            value = "Suc SIS: YES PRI: YES",
            has_setter = false
        },
        {
            id = "version",
            label = {
                text = "Version",
                lang_tag = "gateway_setting_version"
            },
            status = "synced",
            value_type = "string",
            value = "6.04",
            has_setter = false
        },
        {
            id = "unset_pin_codes_count_to_stop_requesting",
            label = {
                text = "Number of sequentially positioned empty pin codes to stop other pin codes requesting",
                lang_tag = "gateway_setting_unset_pin_codes_count_to_stop_requesting.lua"
            },
            status = "synced",
            value_type = "int",
            value = 5,
            value_default = 5,
            value_min = 0,
            value_max = 250,
            has_setter = true
        }
    }
end

return fixture
