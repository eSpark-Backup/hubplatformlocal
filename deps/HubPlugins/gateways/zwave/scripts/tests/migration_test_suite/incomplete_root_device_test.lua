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
local node = {
    basic_type = 0,
    channels = {
        {
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_UNKNOWN",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = true,
    is_smart_include = false,
    manufacturer_id = 0,
    product_id = 0,
    product_type = 0,
    security = {
        "none"
    },
    security_class_started = {
        "none"
    },
    specific_type = "SPECIFIC_TYPE_UNKNOWN"
}
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local node = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/simple_zwave_node_resource" )()
zwave.nodes = {}
zwave.add_node( node )

core.add_device = tests.create_mock()

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

tests.assert_called( core.add_device, "no new device added", nil, 0 )

local expected_devices = {
    {
        name = "Water Valve",
        category = "switch",
        subcategory = "valve",
        node_id = "9000",
        battery_powered = true,
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            }
        }
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )
