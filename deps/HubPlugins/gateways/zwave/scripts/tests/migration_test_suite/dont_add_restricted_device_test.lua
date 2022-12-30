local zwave = require("zwave")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")
zwave.request_node_values = tests.create_mock()

local default_changes = loadfile("HUB:zwave/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true
loadfile = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    1
)

--prepare zwave node
local node = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/simple_zwave_node_resource" )()
zwave.add_node( node )

local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zwave/scripts/helpers")
restrictions_helper:call("storage/insert_restricted_node_id", 9000)

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

tests.assert_eq(core.get_devices(), nil, "Check if devices not added")
tests.assert_eq(#core.get_items(), 0, "Check if items not added")
