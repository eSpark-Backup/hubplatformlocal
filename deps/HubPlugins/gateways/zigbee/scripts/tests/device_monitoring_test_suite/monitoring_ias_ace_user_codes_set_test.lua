local tests = require( "tests" )
local core = require( "core" )
local house_modes = require( "house_modes" )

house_modes.get_current = function() return "1" end

local cluster_ias_ace = {
    name = "ias_ace",
    data_holder = "client",
    attributes = {},
    commands = {
        {
            name = "Arm"
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_ias_ace }
}

local node = {
    id = 42,
    manufacturer_id = "CentraLite",
    model_id = "3400",
    security = 0,
    is_reachable = 1,
    is_battery_powered = true,

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
        item_name = "user_codes",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = ""
    })

tests.assert_ne(item_id, nil, "Try to find item id")


-- test add value
core.update_item_dictionary_value = tests.create_mock()
core.generate_item_dictionary_number_id = function(...)
    return 1
end

loadfile( "HUB:zigbee/scripts/add_item_dictionary_value" )(
    {
        item_id = item_id,
        value_type = "userCode",
        value = {
            code = "1111",
            name = "test1"
        }
    }
)

tests.assert_called(
    core.update_item_dictionary_value,
    "check if item value updated",
    { { item_id, "1", { code = "1111", name = "test1" } } }
)

-- test set value
core.update_item_dictionary_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_dictionary_value" )(
    {
        item_id = item_id,
        value_type = "userCode",
        key = "2",
        value = {
            code = "2222",
            name = "test2"
        }
    }
)

tests.assert_called(
    core.update_item_dictionary_value,
    "check if item value updated",
    { { item_id, "2", { code = "2222", name = "test2" } } }
)

-- test remove value
core.remove_item_dictionary_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/remove_item_dictionary_value" )(
    {
        item_id = item_id,
        value_type = "userCode",
        key = "1"
    }
)

tests.assert_called(
    core.remove_item_dictionary_value,
    "check if item value removed",
    { { item_id, "1" } }
)
