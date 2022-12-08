local tests = require( "tests" )
local core = require( "core" )
local storage = require( "storage" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local default_changes = loadfile("HUB:zwave/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true
loadfile = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

local cc_binary_sensor = { 
    id = class_ids.sensor_binary,
    sensor_types = { 
        0x04 -- co2_alarm
    }
}

local cc_notification = { 
    id = class_ids.notification,
    notifications = { 
        {
            type = "co2_alarm",
            event_types = { "co2_detected" }
        }
    }
}

local cc_alarm_sensor = { 
    id = class_ids.alarm_sensor,
    sensor_types = { "co2_alarm" }
}

local channel_0 = { 
    id = 0,
    classes = { cc_binary_sensor, cc_alarm_sensor, cc_notification }
}

local node = {
    id = "100500",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

local zwave = require( "zwave" )
zwave.request_node_values = tests.create_mock()

-- prepare zwave
zwave.add_node( node )


-- add node
events_handler( { 
    event = "node_added",
    node_id = 100500
} )


local expected_co2_items = {
    {
        name = "co2_alarm",
        tag = "co2_alarm",
        cc_id = class_ids.sensor_binary,
        channel_id = 0
    },
    {
        name = "security_threat",
        tag = "co2_alarm.security_tag",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        main_item_name = "co2_alarm"
    },
    {
        name = "co2_alarm",
        tag = "co2_alarm",
        cc_id = class_ids.alarm_sensor,
        channel_id = 0,
        is_merged = true
    },
    {
        name = "co2_alarm",
        tag = "co2_alarm",
        cc_id = class_ids.notification,
        channel_id = 0,
        is_merged = true
    } 
}

local expected_devices = {
    {
        name = "CO2 Sensor",
        category = "security_sensor",
        subcategory = "co2",
        items = expected_co2_items,
        node_id = "100500",
        battery_powered = true,
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

local check_zwave_node_adding_result = loadfile( test_suite_dir .. "check_zwave_node_adding_result" )

check_zwave_node_adding_result( expected_devices )

core.add_device = tests.create_mock()
core.add_item = tests.create_mock()
core.remove_device_sync = tests.create_mock()
core.remove_device = tests.create_mock()
core.remove_item = tests.create_mock()

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

tests.assert_called( core.add_device, "no new device added", nil, 0 )
tests.assert_called( core.add_item, "no new item added", nil, 0 )
tests.assert_called( core.remove_device_sync, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_device, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_item, "no any item deleted", nil, 0 )


check_zwave_node_adding_result( expected_devices )