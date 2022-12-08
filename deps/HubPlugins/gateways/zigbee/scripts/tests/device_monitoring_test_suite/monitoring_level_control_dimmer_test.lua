local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

timer.set_timeout = tests.create_mock()

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
    clusters = { cluster_level_control }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Bulb",
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
        item_name = "dimmer",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- check dimmer 0
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "level_control",
    attribute = {
        name = "CurrentLevel",
        formatted_value = 0
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, 0 } } )

-- check dimmer 1
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "level_control",
    attribute = {
        name = "CurrentLevel",
        formatted_value = 1
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, 1 } } )

-- check dimmer 100
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "level_control",
    attribute = {
        name = "CurrentLevel",
        formatted_value = 100
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, 39 } } )

-- check dimmer 180
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "level_control",
    attribute = {
        name = "CurrentLevel",
        formatted_value = 180
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, 71 } } )

-- check dimmer 254
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "level_control",
    attribute = {
        name = "CurrentLevel",
        formatted_value = 254
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, 100 } } )
