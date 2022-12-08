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

-- check blue color xy
cluster_color_control.attributes[1].formatted_value = 9830
cluster_color_control.attributes[2].formatted_value = 3932
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "color_control",
    attribute = {
        name = "CurrentX",
        formatted_value = 9830
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, {blue = 254, red = 0, green = 0, cwhite = 0, wwhite = 0} }
    }
)

-- check red color xy
cluster_color_control.attributes[1].formatted_value = 41943
cluster_color_control.attributes[2].formatted_value = 21626
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "color_control",
    attribute = {
        name = "CurrentX",
        formatted_value = 41943
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, {blue = 0, red = 254, green = 0, cwhite = 0, wwhite = 0} }
    }
)

-- check green color xy
cluster_color_control.attributes[1].formatted_value = 19660
cluster_color_control.attributes[2].formatted_value = 39321
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "color_control",
    attribute = {
        name = "CurrentX",
        formatted_value = 19660
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, {blue = 0, red = 0, green = 254, cwhite = 0, wwhite = 0} }
    }
)

-- check temperature cold
cluster_color_control.attributes[4].formatted_value = "color_temperature"
cluster_color_control.attributes[3].formatted_value = 210
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "color_control",
    attribute = {
        name = "ColorTemperatureMireds",
        formatted_value = 210
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, {blue = 0, red = 0, green = 0, cwhite = 195, wwhite = 0} }
    }
)

-- check temperature warm
cluster_color_control.attributes[3].formatted_value = 500
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "color_control",
    attribute = {
        name = "ColorTemperatureMireds",
        formatted_value = 500
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, {blue = 0, red = 0, green = 0, cwhite = 0, wwhite = 90} }
    }
)
