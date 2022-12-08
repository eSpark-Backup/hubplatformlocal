local zwave = require("zwave")
local tests = require("tests")
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
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

local expected_gas_items = {
    {
        name = "test_state",
        cc_id = class_ids.notification,
        tag = "gas_alarm",
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

local expected_devices = {
    {
        name = "Gas Sensor",
        category = "security_sensor",
        subcategory = "gas",
        type = "sensor",
        items = expected_gas_items,
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
        },
        settings = {
            {
                value_type = "action",
                value = {
                    text = "Rediscover",
                    lang_tag = "rediscover_tag"
                },
                tag = "rediscover"
            }
        }
    }
}

local root_device_id = storage:call("find_device_id", node.id)

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

-- modify zwave node
table.insert(
    cc_notification.notifications,
    {
        type = "access_control",
        event_types = {
            "dw_handle_is_opened",
        }
    }
)

local new_expected_devices = {
    {
        name = "Door/Window Sensor",
        category = "security_sensor",
        subcategory = "door",
        type = "sensor",
        items = {
            {
                name = "dw_handle_state",
                cc_id = class_ids.notification,
                tag = "access_control",
                channel_id = 0
            },
        },
        node_id = "9000",
        battery_powered = true,
        subdevices = {
            {
                name = "Gas Sensor",
                category = "security_sensor",
                subcategory = "gas",
                type = "sensor",
                items = expected_gas_items,
            },
            {
                name = "Heat Sensor",
                category = "security_sensor",
                subcategory = "heat",
                type = "sensor",
                items = expected_heat_items,
            },
        },
        settings = {
            {
                value_type = "action",
                value = {
                    text = "Rediscover",
                    lang_tag = "rediscover_tag"
                },
                tag = "rediscover"
            }
        }
    }
}

core.remove_device_sync = tests.create_mock()
core.remove_device = tests.create_mock()
core.remove_item = tests.create_mock()

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

-- check migration result
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( new_expected_devices )

tests.assert_called( core.remove_device_sync, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_device, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_item, "no any item deleted", nil, 0 )

local new_root_device_id = storage:call("find_device_id", node.id)
tests.assert_ne( root_device_id, new_root_device_id, "new root has new id")
