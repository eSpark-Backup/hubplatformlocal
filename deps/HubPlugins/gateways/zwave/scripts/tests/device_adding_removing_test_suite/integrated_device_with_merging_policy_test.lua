local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require "tests"
local core = require( "core" )

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

local check_zwave_node_adding_result = loadfile( test_suite_dir .. "check_zwave_node_adding_result" )
local check_zwave_node_removing_result = loadfile( test_suite_dir .. "check_zwave_node_removing_result" )

local function test_impl(params)

    tests.override_function(_G, "loadfile", function(orig_loadfile, path)
        if path == "HUB:zwave/scripts/model/devices/devices_info/generated/999_133_" .. params.product_id then
            return function()
                local device_info = orig_loadfile("HUB:zwave/scripts/model/devices/devices_info/base_info")()
                device_info.name = 'Test Device'
                device_info.type = 'main_type'
                device_info.category = 'main_category'
                device_info.subcategory = 'main_subcategory'
                device_info.info = {manufacturer = 'Test', model = 'merge policy test'}
                device_info.subdevices_merge_policy = params.merge_policy

                return device_info
            end
        else
            return orig_loadfile(path)
        end
    end)

    zwave.request_node_values = tests.create_mock()


    local node = {
        id = "9000",
        manufacturer_id = 999,
        product_type = 133,
        product_id = params.product_id,
        is_battery_powered = true,

        specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
        generic_type = "GENERIC_TYPE_SWITCH_BINARY",

        security = { "none" },
        security_class_started = { "none" },

        channels = { 
            { 
                id = 0,
                classes = { 
                    { id = class_ids.battery },
                    {
                        id = class_ids.notification,
                        notifications =  {
                            {
                                type = "water_alarm",
                                event_types = { "water_leak_detected" }
                            }
                        },
                    },
                    { 
                        id = class_ids.multilevel_sensor,
                        sensors =  {
                            {
                                scales =  { "celsius" },
                                sensor_type = "air_temperature"
                            }
                        }
                    }
                }
            }
        }
    }

    if params.has_switch_binary_cc then
        table.insert(node.channels[1].classes, { id = class_ids.switch_binary })
    end

    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000 
    } )

    -- check adding result
    params.expected_device.node_id = "9000"
    params.expected_device.battery_powered = true
    params.expected_device.name = "Test Device"

    local check_context = {}

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

    check_zwave_node_adding_result( { params.expected_device }, check_context )

    events_handler( {
        event = "node_removed",
        node_id = 9000,
        manufacturer_id = 999,
        product_type = 133,
        product_id = params.product_id,
    } )

    check_zwave_node_removing_result(check_context)

    tests.reset_overriden_function(_G, "loadfile")
end


local switch_item = {
    name = "switch",
    cc_id = class_ids.switch_binary,
    channel_id = 0
}
local battery_item = {
    name = "battery",
    cc_id = class_ids.battery,
    channel_id = 0
}
local water_leak_alarm_item = {
    name = "water_leak_alarm",
    cc_id = class_ids.notification,
    channel_id = 0,
    tag = "water_alarm"
}
local water_leak_security_threat_item = {
    name = "security_threat",
    cc_id = class_ids.notification,
    channel_id = 0,
    tag = "water_alarm.security_tag",
    main_item_name = "water_leak_alarm"
}
local temperature_item = {
    name = "temp",
    cc_id = class_ids.multilevel_sensor,
    channel_id = 0,
}

tests.run_with_params( test_impl,
{
    {
        value = {
            has_switch_binary_cc = true,
            merge_policy = "merge_all",
            product_id = 1,
            expected_device = {
                category = "main_category",
                subcategory = "main_subcategory",
                items = {
                    switch_item,
                    battery_item,
                    water_leak_alarm_item,
                    water_leak_security_threat_item,
                    temperature_item
                }
            }
        },
        description = "merge_all policy"
    },
    {
        value = {
            has_switch_binary_cc = true,
            merge_policy = "no_separate_root",
            product_id = 2,
            expected_device = {
                category = "security_sensor",
                subcategory = "leak",

                items = {
                    switch_item,
                    battery_item,
                    water_leak_alarm_item,
                    water_leak_security_threat_item
                },
                subdevices = {
                    {
                        name = "Temperature Sensor",
                        category = "temperature",
                        items = {
                            temperature_item
                        }
                    }
                }
            }
        },
        description = "no_separate_root policy"
    },
    {
        value = {
            has_switch_binary_cc = false,
            merge_policy = "separate_root",
            product_id = 3,
            expected_device = {
                items = { battery_item },
                subdevices = {
                    {
                        name = "Water Leak Sensor",
                        category = "security_sensor",
                        subcategory = "leak",
                        items = {
                            water_leak_alarm_item,
                            water_leak_security_threat_item
                        }
                    },
                    {
                        name = "Temperature Sensor",
                        category = "temperature",
                        items = {
                            temperature_item
                        }
                    }
                }
            }
        },
        description = "separate_root policy"
    },
    {
        value = {
            has_switch_binary_cc = false,
            merge_policy = "default",
            product_id = 4,
            expected_device = {
                items = { 
                    battery_item,
                    water_leak_alarm_item,
                    water_leak_security_threat_item
                 },
                subdevices = {
                    {
                        name = "Temperature Sensor",
                        category = "temperature",
                        items = { temperature_item }
                    }
                }
            }
        },
        description = "default policy, no self-sufficient device"
    }
} )
