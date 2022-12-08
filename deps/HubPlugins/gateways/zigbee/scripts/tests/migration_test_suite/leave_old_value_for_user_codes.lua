local tests = require( "tests" )
local storage = require( "storage" )
local core = require( "core" )
local house_modes = require( "house_modes" )

house_modes.get_current = function() return "1" end

local default_changes = loadfile("HUB:zigbee/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true
loadfile = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

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
    manufacturer_id = "Ezlo",
    model_id = "Keypad",
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

-- add user codes
local storage_helper = hubplugins.object( "storage_helper", "HUB:zigbee/scripts/utils" )
local item_id = storage_helper:call(
    "find_item_id",
    {
        item_name = "user_codes",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = ""
    })

tests.assert_ne(item_id, nil, "Try to find item id")

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

-- execute migration
loadfile("HUB:zigbee/scripts/tests/migration_test_suite/run_migration")()

local item_after_migration = core.get_item(item_id)
tests.assert_eq(
    item_after_migration.value,
    {
        ["1"] = {
            code = "1111",
            name = "test1"
        },
        ["2"] = {
            code = "2222",
            name = "test2"
        },
    }
)
