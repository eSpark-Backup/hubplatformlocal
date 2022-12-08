local zwave = require("zwave")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

zwave.request_node_values = tests.create_mock()

local default_changes = loadfile("HUB:zwave/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true
loadfile = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

--prepare zwave node
local node = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/simple_zwave_node_resource" )()
local cc_basic = { 
    id = class_ids.basic,
}
table.insert( node.channels,
    {
        id = 1,
        classes = {cc_basic}
    }
)
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000 
} )

-- check adding result
local expected_switch_items = {
    {
        name = "switch",
        cc_id = class_ids.switch_binary,
        channel_id = 0
    }
}

local basic_item = {
    name = "switch",
    cc_id = class_ids.basic,
    channel_id = 1,
}

local expected_devices = {
    {
        name = "Water Valve",
        category = "switch",
        subcategory = "valve",
        items = expected_switch_items,
        node_id = "9000",
        battery_powered = true,
        subdevices = {
            {
                name = "Water Valve 1",
                category = "switch",
                subcategory = "valve",
                items = {basic_item}
            }
        }
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

-- modify zwave node
zwave.nodes = {}
node.channels[2] = nil
zwave.add_node( node )

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

expected_devices[1].name = "Water Valve"
expected_devices[1].subdevices = nil
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )
