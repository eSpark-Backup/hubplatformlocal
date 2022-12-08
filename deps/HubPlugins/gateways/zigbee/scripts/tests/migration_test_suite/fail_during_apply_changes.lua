local zigbee = require("zigbee")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")

local default_changes = loadfile("HUB:zigbee/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true
local loadfile_with_versions = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

loadfile = function(path)
    if path == "HUB:zigbee/scripts/migration/apply_changes" then
        return function() error("error applying changes" ) end
    else
        return loadfile_with_versions(path)
    end
end

--prepare zigbee node
local node = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/simple_zigbee_node_resource" )()
zigbee.add_node( node )

-- add node
local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )
events_handler( {
    event = "node_added",
    node_id = 42 
} )

-- expected result
local expected_switch_items = {
    {
        name = "switch",
        endpoint_id = 1,
        cluster_name = "on_off",
        attribute_name = "OnOff",
        value_type = "bool",
        value = false
    }
}


local expected_devices = {
    {
        name = "Plug",
        category = "switch",
        subcategory = "interior_plugin",
        items = expected_switch_items,
        node_id = 42,
        battery_powered = false
    }
}

local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices )

-- execute migration
loadfile("HUB:zigbee/scripts/tests/migration_test_suite/run_migration")()

-- check migration result
expected_devices[1].status = "broken"
loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices )
