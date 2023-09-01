local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

zwave.request_node_values = tests.create_mock()

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local base_info = { 
    battery_powered = false,
    category = "sensor",
    gateway_id = "1",
    info = {},
    name = "Sensor",
    type = "device",
}

local specific_info = { 
    battery_powered = false,
    category = "sensor",
    gateway_id = "1",
    info = {},
    name = "Sensor",
    type = "device",
}


local on_get_device_handler_monitoring_impl_called = tests.create_mock()
tests.override_function(hubplugins, "weak_loadfile", function(orig, path)
    if path == "HUB:zwave/scripts/helpers/_get_device_handler_monitoring_impl" then
        on_get_device_handler_monitoring_impl_called()
    elseif path == "HUB:zwave/scripts/model/devices/devices_info/generated/134_258_112" then
        return function()
            return specific_info
        end
    elseif path == "HUB:zwave/scripts/model/devices/devices_info/base_info" then
        return function()
            return base_info
        end
    end



    return orig(path)
end)

do
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

        channels = { 
            {
                id = 0,
                classes = {}
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

    local storage_key = storage:call("get_monitoring_handler_key", 9000)

    tests.assert_eq(storage:call("get_table", storage_key), nil)
    

    local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(9000)

    tests.assert_called(on_get_device_handler_monitoring_impl_called, "check _get_device_handler_monitoring_impl called")

    tests.assert_ne(dev_handler, nil)
    tests.assert_eq(storage:call("get_table", storage_key), { name = "default" })

    dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(9000)
    tests.assert_ne(dev_handler, nil)
    tests.assert_called(on_get_device_handler_monitoring_impl_called, "check _get_device_handler_monitoring_impl not called", {})

    tests.assert_eq(dev_handler:call("get_device_info"), base_info)

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",     
        manufacturer_id = 1,
        product_type = 2,
        product_id = 3, 
    } )

    tests.assert_eq(storage:call("get_table", storage_key), nil)
end

do
    local node = {
        id = "100500",
        manufacturer_id = 134,
        product_type = 258,
        product_id = 112,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",

        security = { "none" },
        security_class_started = { "none" },

        channels = { 
            {
                id = 0,
                classes = {}
            }
        }
    }
        
    -- prepare zwave
    zwave.add_node( node )
    
    -- add node
    events_handler( {
        event = "node_added",
        node_id = 100500 
    } )

    local storage_key = storage:call("get_monitoring_handler_key", 100500)

    tests.assert_eq(storage:call("get_table", storage_key), nil)
    

    local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(100500)

    tests.assert_called(on_get_device_handler_monitoring_impl_called, "check _get_device_handler_monitoring_impl called")

    tests.assert_ne(dev_handler, nil)
    tests.assert_eq(storage:call("get_table", storage_key), { name = "134_258_112" })

    dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(100500)
    tests.assert_ne(dev_handler, nil)
    tests.assert_called(on_get_device_handler_monitoring_impl_called, "check _get_device_handler_monitoring_impl not called", {})

    tests.assert_eq(dev_handler:call("get_device_info"), specific_info)

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "100500",     
        manufacturer_id = 134,
        product_type = 228,
        product_id = 112, 
    } )

    tests.assert_eq(storage:call("get_table", storage_key), nil)
end
