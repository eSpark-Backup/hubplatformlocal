local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local reset_setting = loadfile( "HUB:zwave/scripts/reset_setting" )

zwave.request_node_values = tests.create_mock()
local set_setting_value_mock = tests.create_mock()

local some_specific_cc_id = class_ids.switch_binary

tests.override_function(_G, "loadfile", function(orig_loadfile, path)
    local orig_loader = orig_loadfile(path)
    if path == "HUB:zwave/scripts/model/devices/monitoring/default/set_setting_value" then
        return function(self, setting_id, value)
            return set_setting_value_mock(setting_id, value)
        end
    elseif path == "HUB:zwave/scripts/model/devices/inclusion/default/create_default_settings" then
        return function(self, device_data, zwave_node_id)
            orig_loader(self, device_data, zwave_node_id)

            table.insert(device_data.settings, {
                label = { text = "bool setting" },
                description = { text = "description" },
                value_type = "bool",
                value = false,
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "bool"
                }
            })


            table.insert(device_data.settings, {
                label = { text = "int setting" },
                description = { text = "description" },
                value_type = "int",
                value = 1,
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "int"
                }
            })

            table.insert(device_data.settings, {
                label = { text = "float setting" },
                description = { text = "description" },
                value_type = "float",
                value = 1,
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "float"
                }
            })


            table.insert(device_data.settings, {
                label = { text = "string setting" },
                description = { text = "description" },
                value_type = "string",
                value = 1,
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "string"
                }
            })

            table.insert(device_data.settings, {
                label = { text = "temperature setting" },
                description = { text = "description" },
                value_type = "temperature",
                value = {
                    value = 1,
                    scale = "celsius"
                },
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "temperature"
                }
            })

            table.insert(device_data.settings, {
                label = { text = "array setting" },
                description = { text = "description" },
                value_type = "array.int",
                value = { },
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "array"
                }
            })

            table.insert(device_data.settings, {
                label = { text = "setting" },
                description = { text = "description" },
                value_type = "int",
                value = 1,
                value_default = 100,
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "default"
                }
            })

            table.insert(device_data.settings, {
                label = { text = "action setting" },
                description = { text = "description" },
                value_type = "action",
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "action"
                }
            })

            table.insert(device_data.settings, {
                label = { text = "token setting" },
                description = { text = "description" },
                value_type = "token",
                enum = { 
                    unknown = { text = "U" },
                    x = { text = "X" },
                    y = { text = "Y" },
                    z = { text = "Z" }
                },
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "token"
                }
            })

            table.insert(device_data.settings, {
                label = { text = "token setting" },
                description = { text = "description" },
                value_type = "token",
                enum = { 
                    unknown = { text = "U" }
                },
                descriptor = {
                    node_id = 9000,
                    class_id = some_specific_cc_id,
                    tag = "incomplete_token"
                }
            })
        end
    else
        return orig_loader
    end
end)


local node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { 
        { 
            id = 0,
            classes = { 
                { id = some_specific_cc_id }
            }
        }
    }
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000 
} )

local device_id = assert(storage:call("find_device_id", 9000), "device not found")

local function find_setting_id(tag)
    return assert(storage:call("find_setting_id", 9000, some_specific_cc_id, tag), "setting " .. tag .. " not found")
end

do
    reset_setting({ setting_id = find_setting_id("int") })
    tests.assert_called(set_setting_value_mock, "check integer value", { { find_setting_id("int"), 0 } })
end

do
    reset_setting({ setting_id = find_setting_id("float") })
    tests.assert_called(set_setting_value_mock, "check float value", { { find_setting_id("float"), 0 } })
end

do
    reset_setting({ setting_id = find_setting_id("string") })
    tests.assert_called(set_setting_value_mock, "check string value", { { find_setting_id("string"), "" } })
end

do
    reset_setting({ setting_id = find_setting_id("temperature") })
    tests.assert_called(set_setting_value_mock, "check temperature value", { { find_setting_id("temperature"), { value = 0, scale = "celsius" } } })
end

do
    reset_setting({ setting_id = find_setting_id("array") })
    tests.assert_called(set_setting_value_mock, "check array value", { { find_setting_id("array"), { } } })
end

do
    reset_setting({ setting_id = find_setting_id("default") })
    tests.assert_called(set_setting_value_mock, "check default value", { { find_setting_id("default"), 100 } })
end

do
    reset_setting({ setting_id = find_setting_id("token") })
    tests.assert_called(set_setting_value_mock, "check token value", { { find_setting_id("token"), "x" } })
end

do
    reset_setting({ setting_id = find_setting_id("action") })
    tests.assert_called(set_setting_value_mock, "check set action value not called", { })
end

do
    reset_setting({ setting_id = find_setting_id("incomplete_token") })
    tests.assert_called(set_setting_value_mock, "check set incomplete token value not called", { })
end

do
    reset_setting({ device_id = device_id })
    tests.assert_called_unordered(set_setting_value_mock, "check values", { 
        { find_setting_id("bool"), true },
        { find_setting_id("int"), 0 },
        { find_setting_id("float"), 0 },
        { find_setting_id("string"), "" },
        { find_setting_id("temperature"), { value = 0, scale = "celsius" } },
        { find_setting_id("array"), { } },
        { find_setting_id("default"), 100 },
        { find_setting_id("token"), "x" } 
    })
end

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",     
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3
} )
