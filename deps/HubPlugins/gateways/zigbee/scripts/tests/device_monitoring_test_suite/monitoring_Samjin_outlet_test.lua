local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

timer.set_timeout = tests.create_mock()

local cluster_on_off = {
    name = "on_off",
    attributes = {
        {
            name = "OnOff",
            formatted_value = "false"
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
    clusters = { cluster_on_off, cluster_level_control }
}

local node = {
    id = 42,
    manufacturer_id = "Samjin",
    model_id = "outlet",
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
        item_name = "switch",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "on_off",
        attribute_name = "OnOff"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- check switch enabled
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "on_off",
    attribute = {
        name = "OnOff",
        formatted_value = "true"
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, true } } )

-- check switch disabled
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "on_off",
    attribute = {
        name = "OnOff",
        formatted_value = "false"
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, false } } )

-- check level control update with disabled switch
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

tests.assert_called( core.update_item_value, "check if item value updated", { { item_id, true } } )

-- check level control update with enabled switch
cluster_on_off.attributes[1].formatted_value = "true"
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

tests.assert_called( core.update_item_value, "check if item value updated", { { item_id, true } } )

-- check level control 0 value
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

tests.assert_called( core.update_item_value, "check if item value updated", { { item_id, false } } )

-- check level control 1 value
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

tests.assert_called( core.update_item_value, "check if item value updated", { { item_id, true } } )

-- check level control 50 value
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "level_control",
    attribute = {
        name = "CurrentLevel",
        formatted_value = 50
    }
} )

tests.assert_called( core.update_item_value, "check if item value updated", { { item_id, true } } )
