local zigbee = require("zigbee")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")

local default_changes = loadfile("HUB:zigbee/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true

local prev_loadfile = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)
loadfile = function(path)
    if path == "HUB:zigbee/scripts/helpers/device_info_detection/items_based/ensure_plug" then
        return function () 
            return {
                name = "Dimmer",
                category = "dimmable_light",
                subcategory = "dimmable_bulb"
            }
        end
    else
        return prev_loadfile(path)
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

core.add_device = tests.create_mock()
core.add_item = tests.create_mock()
core.remove_device_sync = tests.create_mock()
core.remove_device = tests.create_mock()
core.remove_item = tests.create_mock()
core.modify_device = tests.create_mock()
core.modify_item = tests.create_mock()

loadfile = function(path)
    if path == "HUB:zigbee/scripts/helpers/device_info_detection/items_based/ensure_plug" then
        return function () 
            return {
                name = "Plug",
                category = "switch",
                subcategory = "interior_plugin"
            }
        end
    else
        return prev_loadfile( path )
    end
end

-- execute migration
loadfile("HUB:zigbee/scripts/tests/migration_test_suite/run_migration")()

tests.assert_called( core.add_device, "no new device added", nil, 0 )
tests.assert_called( core.add_item, "no new item added", nil, 0 )
tests.assert_called( core.remove_device_sync, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_device, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_item, "no any item deleted", nil, 0 )

tests.assert_called( core.modify_device, "root device modified", nil, 1 )
tests.assert_called( core.modify_item, "item modified", nil, 1 )
