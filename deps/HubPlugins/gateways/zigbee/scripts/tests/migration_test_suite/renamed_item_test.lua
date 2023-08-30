local zigbee = require("zigbee")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local default_changes = loadfile("HUB:zigbee/scripts/migration/version_changes/default")()
default_changes.clusters_to_check = {on_off = true}
default_changes.items_old_names = {["electric_meter_volt"] = "electric_meter_amper"}
loadfile = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

--prepare zigbee node
local node = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/simple_zigbee_node_resource" )()
table.insert(
    node.endpoints[1].clusters,
    {
        name = "electrical_measurement",
        attributes = {
            {
                name = "RMSCurrent",
                formatted_value = 0
            }
        }
    })

zigbee.add_node( node )

-- add node
local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

item_names.electric_meter_amper = "electric_meter_volt"
events_handler( {
    event = "node_added",
    node_id = 42
} )

-- check adding result
local expected_switch_items = {
    {
        name = "switch",
        endpoint_id = 1,
        cluster_name = "on_off",
        attribute_name = "OnOff"
    },
    {
        name = "electric_meter_volt",
        endpoint_id = 1,
        cluster_name = "electrical_measurement",
        attribute_name = "RMSCurrent"
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

-- modify item name
item_names.electric_meter_amper = "electric_meter_amper"

core.add_device = tests.create_mock()
core.add_item = tests.create_mock()
core.remove_device_sync = tests.create_mock()
core.remove_device = tests.create_mock()
core.remove_item = tests.create_mock()

-- execute migration
loadfile("HUB:zigbee/scripts/tests/migration_test_suite/run_migration")()

-- check migration result
expected_devices[1].items[2].name = "electric_meter_amper"
loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices )

tests.assert_called( core.add_device, "no new device added", nil, 0 )
tests.assert_called( core.add_item, "no new item added", nil, 0 )
tests.assert_called( core.remove_device_sync, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_device, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_item, "no any item deleted", nil, 0 )
