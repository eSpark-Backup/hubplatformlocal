local zwave = require("zwave")
local core = require("core")
local timer = require("timer")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )

zwave.request_node_values = tests.create_mock()
zwave.request_value = tests.create_mock()
zwave.set_rgb_value = tests.create_mock()
zwave.color_switch_level_change = tests.create_mock()

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

local function get_item_id(item_name)
    return storage:call("find_item_id", item_name, 9000, class_ids.switch_color, 0 ) 
end

local function get_color_item_value()
    return core.get_item( get_item_id(item_names.rgb_color) ).value
end

-- set components
do
    set_item_value({
        item_id = get_item_id(item_names.rgb_color),
        value = { red = 50, green = 255, blue = 128 }
    })

    tests.assert_called(zwave.set_rgb_value, "check set rgb value called", { { "9000", class_ids.switch_color, 0, { red = 50, green = 255, blue = 128 } } })
    tests.assert_called(zwave.request_value, "check request value called", { { "9000", class_ids.switch_color, 0 } })
end

-- set component up
do
    set_item_value({
        item_id = get_item_id(item_names.rgb_color_up),
        value = "green"
    })

    tests.assert_called(zwave.color_switch_level_change, "check level change up called", { { "9000", 0, "green", "up" } })
    tests.assert_called_postponed(timer.add_time, 1200, zwave.request_value, "check request value called", { { "9000", class_ids.switch_color, 0 } })
end

-- set component down
do
    set_item_value({
        item_id = get_item_id(item_names.rgb_color_down),
        value = "blue"
    })

    tests.assert_called(zwave.color_switch_level_change, "check level change down called", { { "9000", 0, "blue", "down" } })
    tests.assert_called_postponed(timer.add_time, 1200, zwave.request_value, "check request value called", { { "9000", class_ids.switch_color, 0 } })
end

-- set component stop
do
    set_item_value({
        item_id = get_item_id(item_names.rgb_color_stop),
        value = "red"
    })

    tests.assert_called(zwave.color_switch_level_change, "check level change stop called", { { "9000", 0, "red", "stop" } })
    tests.assert_called_postponed(timer.add_time, 500, zwave.request_value, "check request value called", { { "9000", class_ids.switch_color, 0 } })
end

-- set all components stop
do
    set_item_value({
        item_id = get_item_id(item_names.rgb_color_stop),
        value = "all"
    })

    tests.assert_called_unordered(zwave.color_switch_level_change, 
        "check all level change stop called", 
        { 
            { "9000", 0, "red", "stop" },
            { "9000", 0, "green", "stop" },
            { "9000", 0, "blue", "stop" },
            { "9000", 0, "wwhite", "stop" } 
        })

    tests.assert_called_postponed(timer.add_time, 500, zwave.request_value, "check request value called", { { "9000", class_ids.switch_color, 0 } })
end