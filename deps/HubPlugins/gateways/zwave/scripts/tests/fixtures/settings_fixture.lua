local tests = require("tests")
local core = require("core")
local zwave = require("zwave")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local events_handler = loadfile("HUB:zwave/scripts/events/events_handling")
local set_setting_value = loadfile("HUB:zwave/scripts/set_setting_value")
local set_setting_dictionary_value = loadfile("HUB:zwave/scripts/set_setting_dictionary_value")
local reset_setting = loadfile("HUB:zwave/scripts/reset_setting")

core.set_setting_value = tests.track_calls(core.set_setting_value)
core.set_setting_dictionary_value = tests.track_calls(core.set_setting_dictionary_value)
core.update_setting_status = tests.create_mock()
core.update_setting_dictionary_status = tests.create_mock()

zwave.add_configuration = tests.create_mock()
zwave.remove_configuration = tests.create_mock()
zwave.request_node_values = tests.create_mock()
zwave.set_configuration_value = tests.create_mock()
zwave.reset_configuration_value = tests.create_mock()
zwave.request_configuration_value = tests.create_mock()
zwave.reset_all_configuration_values = tests.create_mock()

local mocked_files = {}
local function mock_file(path, content)
    mocked_files[path] = content
end

tests.override_function( hubplugins, "weak_loadfile", function(original_loadfile, path)
    local mock_loadfile = mocked_files[path]
    return mock_loadfile or original_loadfile(path)
end)

tests.override_function( hubplugins, "weak_require", function(original_require, path)
    local mock_loadfile = mocked_files[path]
    return mock_loadfile and mock_loadfile() or original_require(path)
end )

local fixture = {
    node_id = "9000"
}

function fixture.get_device_id()
    return core.get_devices()[1].id
end

local default_settings_count = 1

function fixture.get_setting_id(idx)
    return core.get_settings()[(idx or 1) + default_settings_count + 1].id
end

function fixture.get_generic_setting_id()
    return core.get_settings()[default_settings_count + 1].id
end

fixture.rediscovering_configuration = {
    label = {
        text = "Rediscover device"
    },
    description = {
        text = "Rediscover (repeat interview) device"
    },
    value_type = "action",
    value = {
        text = "Rediscover",
        lang_tag = "rediscover_tag"
    }
}

fixture.generic_zwave_configurations = {
    label = {
        lang_tag = "zwave_device_configuration",
        text = "Z-Wave Configurations"
    },
    description = {
        lang_tag = "zwave_device_configuration_descr",
        text = "Specific Z-Wave configuration parameters"
    },
    value_type = "dictionary.zwave_configuration",
    status = "pending",
    value = {}
}

function fixture.set_device_settings(settings)
    mock_file("HUB:zwave/scripts/helpers/get_device_info", function(dev_node_id)
        local device_info = hubplugins.weak_require("HUB:zwave/scripts/model/devices/devices_info/base_info")

        device_info.settings_count = #settings

        return device_info
    end)

    mock_file("HUB:zwave/scripts/helpers/get_device_setting_info", function(dev_node_id, idx)
        return settings[idx]
    end)
end

local node = {
    id = fixture.node_id,
    manufacturer_id = 0,
    product_type = 0,
    product_id = 0,
    is_battery_powered = false,
    specific_type = "",
    generic_type = "",
    security = { "s0" },
    security_class_started = { "s0" },
    channels = {
        {
            id = 0,
            classes = {
                {
                    id = class_ids.configuration,
                    version = 1
                }
            }
        }
    }
}

function fixture.set_configurations_version(version)
    node.channels[1].classes[1].version = version
end

function fixture.set_configurations_v3_parameters(params)
    node.channels[1].classes[1].parameters = params
end

function fixture.change_node_manufacturer_id(manufacturer_id, product_type, product_id)
    node.manufacturer_id = manufacturer_id
    node.product_type = product_type
    node.product_id = product_id
end

function fixture.add_node()
    zwave.add_node(node)

    events_handler({
        event = "node_added",
        node_id = fixture.node_id
    })
end

function fixture.remove_node()
    events_handler({
        event = "node_removed",
        node_id = node.id,
        manufacturer_id = node.manufacturer_id,
        product_type = node.product_type,
        product_id = node.product_id
    })
end

function fixture.check_device_settings(settings)
    local actual_settings = core.get_settings()

    tests.assert_eq(#actual_settings, #settings, "settings count")

    local device_id = fixture.get_device_id()

    for idx, actual_setting in ipairs(actual_settings) do
        local expected_setting = settings[idx]
        expected_setting.device_id = device_id
        tests.assert_eq(actual_setting, expected_setting)
    end
end

function fixture.set_setting_value(value)
    set_setting_value({
        setting_id = fixture.get_setting_id(),
        value = value
    })
end

function fixture.set_setting_dictionary_value(key, value)
    set_setting_dictionary_value({
        setting_id = fixture.get_generic_setting_id(),
        key = key,
        value = value
    })
end

function fixture.reset_setting(id)
    reset_setting({
        setting_id = id
    })
end

function fixture.reset_settings()
    reset_setting({
        device_id = fixture.get_device_id()
    })
end

function fixture.update_configuration_value(number, size, value)
    events_handler({
        event = "configuration_value_updated",
        node_id = fixture.node_id,
        channel_id = 0,
        class_id = class_ids.configuration,
        parameter_num = number,
        parameter_size = size,
        value = value
    })
end

function fixture.update_configuration_status(number, status)
    events_handler({
        event = "configuration_request_status_updated",
        node_id = fixture.node_id,
        parameter_num = number,
        status = status
    })
end

function fixture.check_add_configuration_called(...)
    local args_lists = { ... }

    for _, args_list in pairs(args_lists) do
        table.insert(args_list, 1, fixture.node_id)
    end

    tests.assert_called(zwave.add_configuration, "check calls to zwave.add_configuration", args_lists)
end

function fixture.check_remove_configuration_called(number)
    tests.assert_called(zwave.remove_configuration, "check calls to zwave.remove_configuration", {
        { fixture.node_id, number }
    })
end

function fixture.check_set_configuration_value_called(number, size, value)
    tests.assert_called(zwave.set_configuration_value, "check calls to zwave.set_configuration_value", {
        { fixture.node_id, number, size, value }
    })
end

function fixture.check_set_configuration_value_not_called()
    tests.assert_called(zwave.set_configuration_value, "check calls to zwave.set_configuration_value", {})
end

function fixture.check_reset_configuration_value_called(number)
    tests.assert_called(zwave.reset_configuration_value, "check calls to zwave.reset_configuration_value", {
        { fixture.node_id, number }
    })
end

function fixture.check_reset_configuration_value_not_called(number)
    tests.assert_called(zwave.reset_configuration_value, "check calls to zwave.reset_configuration_value", {})
end

function fixture.check_reset_all_configuration_values_called()
    tests.assert_called(zwave.reset_all_configuration_values, "check calls to zwave.reset_all_configuration_values", {
        { fixture.node_id }
    })
end

function fixture.check_request_configuration_value_called(number)
    tests.assert_called(zwave.request_configuration_value, "check calls to zwave.request_configuration_value", {
        { fixture.node_id, number }
    })
end

function fixture.check_set_setting_value_called(value, status)
    tests.assert_called(core.set_setting_value, "check calls to core.set_setting_value", {
        { fixture.get_setting_id(), value, status }
    })
end

function fixture.check_set_setting_value_not_called()
    tests.assert_called(core.set_setting_value, "check calls to core.set_setting_value", {})
end

function fixture.check_set_setting_dictionary_value_called(key, value)
    tests.assert_called(core.set_setting_dictionary_value, "check calls to core.set_setting_dictionary_value", {
        { fixture.get_generic_setting_id(), key, value }
    })
end

function fixture.check_update_setting_status_called(status)
    tests.assert_called(core.update_setting_status, "check calls to core.update_setting_status", {
        { fixture.get_setting_id(), status }
    })
end

function fixture.check_update_setting_dictionary_status_called(key, status)
    tests.assert_called(core.update_setting_dictionary_status, "check calls to core.update_setting_dictionary_status", {
        { fixture.get_generic_setting_id(), key, status }
    })
end

return fixture
