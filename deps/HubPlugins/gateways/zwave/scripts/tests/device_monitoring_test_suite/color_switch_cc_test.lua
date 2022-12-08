local zwave = require("zwave")
local core = require("core")
local timer = require("timer")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()
zwave.request_value = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { 
        {
            id = class_ids.switch_color,
            version = 2,
            component_ids = { "red", "green", "blue", "wwhite" }
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

local function get_color_item_value()
    return core.get_item( storage:call("find_item_id", item_names.rgb_color, 9000, class_ids.switch_color, 0 ) ).value
end

-- report without targets + value changed
do
    events_handler( {
        event = "rgb_value_updated",
        node_id = 9000,
        class_id = class_ids.switch_color,
        channel_id = 0,
        colors = {
            red = 30,
            green = 40,
            blue = 50,
            wwhite = 60
        },
        target_colors = { },
        target_durations = { }
    } )

    tests.assert_eq(
        get_color_item_value(), 
        {  
            red = 30,
            green = 40,
            blue = 50,
            wwhite = 60
        })

    tests.assert_called_postponed(
        timer.add_time,
        2000,
        zwave.request_value,
        "check value requested",
        { { 9000, class_ids.switch_color, 0 } })
end

-- report without targets + value not changed
do
    events_handler( {
        event = "rgb_value_updated",
        node_id = 9000,
        class_id = class_ids.switch_color,
        channel_id = 0,
        colors = {
            red = 30,
            green = 40,
            blue = 50,
            wwhite = 60
        },
        target_colors = { },
        target_durations = { }
    } )

    tests.assert_eq(
        get_color_item_value(), 
        {  
            red = 30,
            green = 40,
            blue = 50,
            wwhite = 60
        })

    tests.assert_called_postponed(
        timer.add_time,
        2000,
        zwave.request_value,
        "check value not requested",
        { })
end

-- report with targets + value not changed
do
    events_handler( {
        event = "rgb_value_updated",
        node_id = 9000,
        class_id = class_ids.switch_color,
        channel_id = 0,
        colors = {
            red = 30,
            green = 40,
            blue = 50,
            wwhite = 60
        },
        target_colors = { 
            red = 31,
            green = 40,
            blue = 50,
            wwhite = 60
        },
        target_durations = { }
    } )

    tests.assert_eq(
        get_color_item_value(), 
        {  
            red = 30,
            green = 40,
            blue = 50,
            wwhite = 60
        })

    tests.assert_called_postponed(
        timer.add_time,
        2000,
        zwave.request_value,
        "check value requested",
        { { 9000, class_ids.switch_color, 0 } })
end


-- check not all components reported 
do
    events_handler( {
        event = "rgb_value_updated",
        node_id = 9000,
        class_id = class_ids.switch_color,
        channel_id = 0,
        colors = { red = 34, green = 22 },
        target_colors = { red = 31 },
        target_durations = { }
    } )

    tests.assert_eq(
        get_color_item_value(), 
        {  
            red = 34,
            green = 22,
            blue = 50,
            wwhite = 60
        })
end
