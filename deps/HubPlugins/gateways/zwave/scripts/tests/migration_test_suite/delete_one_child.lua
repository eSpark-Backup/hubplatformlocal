local zwave = require("zwave")
local tests = require("tests")
local core = require( "core" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

zwave.request_node_values = tests.create_mock()

local default_changes = loadfile("HUB:zwave/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true
loadfile = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(
    require( "storage" ),
    default_changes,
    1,
    2
)

local cc_notification = {
    id = class_ids.notification,
    notifications = {
        {
            type = "heat_alarm",
            event_types = {
                "rapid_temperature_rise",
            }
        },
        {
            type = "gas_alarm",
            event_types = {
                "gas_alarm_test",
            }
        },
        {
            type = "access_control",
            event_types = {
                "dw_handle_is_opened",
            }
        },
    }
}

local channel_0 = {
    id = 0,
    classes = { cc_notification }
}

local node = {
    id = "9000",
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

--prepare zwave node
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local expected_dw_items = {
    {
        name = "dw_handle_state",
        cc_id = class_ids.notification,
        tag = "access_control",
        channel_id = 0
    },
}

local expected_heat_items = {
    {
        name = "temperature_changes",
        cc_id = class_ids.notification,
        tag = "heat_alarm",
        channel_id = 0
    },
}

local expected_gas_items = {
    {
        name = "test_state",
        cc_id = class_ids.notification,
        tag = "gas_alarm",
        channel_id = 0
    },
}

local expected_devices = {
    {
        name = "Door/Window Sensor",
        category = "security_sensor",
        subcategory = "door",
        type = "sensor",
        items = expected_dw_items,
        node_id = "9000",
        battery_powered = true,
        subdevices = {
            {
                name = "Heat Sensor",
                category = "security_sensor",
                subcategory = "heat",
                type = "sensor",
                items = expected_heat_items,
            },
            {
                name = "Gas Sensor",
                category = "security_sensor",
                subcategory = "gas",
                type = "sensor",
                items = expected_gas_items,
            }
        }
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

-- modify zwave node
table.remove( cc_notification.notifications, 2 ) -- remove gas_alarm

local new_expected_devices = {
    {
        name = "Door/Window Sensor",
        category = "security_sensor",
        subcategory = "door",
        type = "sensor",
        items = expected_dw_items,
        node_id = "9000",
        battery_powered = true,
        subdevices = {
            {
                name = "Heat Sensor",
                category = "security_sensor",
                subcategory = "heat",
                type = "sensor",
                items = expected_heat_items,
            },
        }
    }
}

core.add_device = tests.create_mock()
core.add_item = tests.create_mock()

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

-- check migration result
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( new_expected_devices )

tests.assert_called( core.add_device, "no new device added", nil, 0 )
tests.assert_called( core.add_item, "no new item added", nil, 0 )