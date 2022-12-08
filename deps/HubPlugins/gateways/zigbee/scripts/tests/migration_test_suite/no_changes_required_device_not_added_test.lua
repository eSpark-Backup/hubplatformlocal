local zigbee = require("zigbee")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")

local default_changes = loadfile("HUB:zigbee/scripts/migration/version_changes/default")()
loadfile = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

--prepare zigbee node
local node = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/simple_zigbee_node_resource" )()
zigbee.add_node( node )

-- execute migration
loadfile("HUB:zigbee/scripts/tests/migration_test_suite/run_migration")()

tests.assert_eq(#core.get_devices(), 1, "Check if devices added")
tests.assert_eq(#core.get_items(), 1, "Check if items added")
