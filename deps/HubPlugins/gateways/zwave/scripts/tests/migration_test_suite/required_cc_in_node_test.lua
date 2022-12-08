local zwave = require("zwave")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

zwave.request_node_values = tests.create_mock()

local default_changes = loadfile("HUB:zwave/scripts/migration/version_changes/default")()
default_changes.cc_to_check = {[class_ids.switch_binary] = true}
loadfile = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

--prepare zwave node
local node = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/simple_zwave_node_resource" )()
zwave.add_node( node )

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

tests.assert_eq(#core.get_devices(), 1, "Check if devices added")
tests.assert_eq(#core.get_items(), 1, "Check if items added")
