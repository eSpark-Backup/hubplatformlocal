local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

timer.set_timeout = tests.create_mock()

local cluster_color_control = {
    name = "color_control",
    attributes = {
        {
            name = "CurrentX",
            formatted_value = 0
        },
        {
            name = "CurrentY",
            formatted_value = 0
        },
        {
            name = "ColorTemperatureMireds",
            formatted_value = 0
        },
        {
            name = "ColorMode",
            formatted_value = "color_xy"
        },
        {
            name = "ColorTempPhysicalMinMireds",
            formatted_value = 150
        },
        {
            name = "ColorTempPhysicalMaxMireds",
            formatted_value = 666
        }
    }
}

local cluster_level_control = {
    name = "level_control",
    attributes = {
        {
            name = "CurrentLevel",
            formatted_value = 0
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_color_control, cluster_level_control }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "RGB Bulb",
    security = 0,
    is_reachable = 1,
    is_battery_powered = false,

    endpoints = { endpoint_1 }
}

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

local zigbee = require( "zigbee" )

-- prepare zigbee
zigbee.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 42
} )

local storage = hubplugins.object( "storage_helper", "HUB:zigbee/scripts/utils" )
local item_id = storage:call(
    "find_item_id",
    {
        item_name = "rgbcolor",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "color_control",
        attribute_name = ""
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- check set zero
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {blue = 0, red = 0, green = 0, cwhite = 0, wwhite = 0}} )

tests.assert_called( zigbee.send_command, "check if on command sent", {} )

-- check set blue
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {blue = 255, red = 0, green = 0, cwhite = 0, wwhite = 0}} )

tests.assert_called( zigbee.send_command, "check if command sent", { { 42, 1, "color_control", "MoveToColor", {current_x = 9830, current_y = 3932} } } )

-- check set red
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {blue = 0, red = 255, green = 0, cwhite = 0, wwhite = 0}} )

tests.assert_called( zigbee.send_command, "check if command sent", { { 42, 1, "color_control", "MoveToColor", {current_x = 41943, current_y = 21626} } } )

-- check set green
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {blue = 0, red = 0, green = 255, cwhite = 0, wwhite = 0}} )

tests.assert_called( zigbee.send_command, "check if command sent", { { 42, 1, "color_control", "MoveToColor", {current_x = 19660, current_y = 39321} } } )

-- check set cold 255 white
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {blue = 0, red = 0, green = 0, cwhite = 255, wwhite = 0}} )

tests.assert_called( zigbee.send_command, "check if command sent", { { 42, 1, "color_control", "MoveToColorTemperature", 150 } } )

-- check set cold 100 white
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {blue = 0, red = 0, green = 0, cwhite = 100, wwhite = 0}} )

tests.assert_called( zigbee.send_command, "check if command sent", { { 42, 1, "color_control", "MoveToColorTemperature", 306 } } )

-- check set warm 255 white
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {blue = 0, red = 0, green = 0, cwhite = 0, wwhite = 255}} )

tests.assert_called( zigbee.send_command, "check if command sent", { { 42, 1, "color_control", "MoveToColorTemperature", 666 } } )

-- check set warm 100 white
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {blue = 0, red = 0, green = 0, cwhite = 0, wwhite = 100}} )

tests.assert_called( zigbee.send_command, "check if command sent", { { 42, 1, "color_control", "MoveToColorTemperature", 509 } } )

