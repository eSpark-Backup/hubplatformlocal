local zwave = require("zwave")
local core = require("core")
local storage = require("storage")
local storage_helper= hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local tests = require("tests")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

zwave.request_node_values = tests.create_mock()

local default_changes = loadfile("HUB:zwave/scripts/migration/version_changes/default")()
default_changes.items_to_change_value = {
    ["switch"] = true
}
default_changes.check_all_nodes = true
loadfile = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

--prepare zwave node
local node = loadfile("HUB:zwave/scripts/tests/migration_test_suite/simple_zwave_node_resource")()
zwave.add_node(node)

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000 
} )

local item_id = storage_helper:call("find_item_id", 
    "switch", 
    "9000", 
    class_ids.switch_binary, 
    node.channels[1].id
)

tests.assert_ne(item_id, nil)

core.update_item_value(item_id, true)

tests.assert_eq(core.get_item(item_id).value, true)

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

tests.assert_eq(core.get_item(item_id).value, false)
