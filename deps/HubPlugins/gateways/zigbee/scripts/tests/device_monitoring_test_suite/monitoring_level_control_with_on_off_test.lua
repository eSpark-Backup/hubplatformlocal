local tests = require( "tests" )
local core = require( "core" )
local zigbee = require( "zigbee" )
local timer = require( "timer" )

local cluster_level_control = {
    name = "level_control",
    attributes = {
        {
            name = "CurrentLevel",
            formatted_value = 0
        }
    }
}

local cluster_on_off = {
    name = "on_off",
    attributes = {
        {
            name = "OnOff",
            formatted_value = "false"
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_level_control, cluster_on_off }
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
local dimmer_item_id = storage:call(
    "find_item_id",
    {
        item_name = "dimmer",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel"
    })

tests.assert_ne(dimmer_item_id, nil, "Try to find item id")

local switch_item_id = storage:call(
    "find_item_id",
    {
        item_name = "switch",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "on_off",
        attribute_name = "OnOff"
    })

tests.assert_ne(switch_item_id, nil, "Try to find item id")

-- check dimmer not updated when swicth is off
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "level_control",
    attribute = {
        name = "CurrentLevel",
        formatted_value = 10
    }
} )

tests.assert_called( core.update_item_value, "check if item values not updated", {} )

-- check switch turned on
zigbee.request_attribute_value = tests.create_mock()
timer.set_timeout = tests.create_mock()

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

tests.assert_called(
    timer.set_timeout,
    "check if dimmer value requested",
        {
            {
                1000,
                "HUB:zigbee/scripts/timer_handlers/timeout/request_attribute_value",
                {
                    node_id = 42,
                    endpoint_id = 1,
                    cluster_name = "level_control",
                    attribute_name = "CurrentLevel"
                }
            }
        }
)

-- check switch turned off
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

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { dimmer_item_id, 0 },
        { switch_item_id, false }
    } )

-- check dimmer updated when switch turned on
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

tests.assert_called( core.update_item_value, "check if item values updated", { { dimmer_item_id, 39 } } )

-- check dimmer updated to 1 when switch turned on and receive dimmer 0
cluster_on_off.attributes[1].formatted_value = "true"
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

tests.assert_called( core.update_item_value, "check if item values updated", { { dimmer_item_id, 1 } } )
