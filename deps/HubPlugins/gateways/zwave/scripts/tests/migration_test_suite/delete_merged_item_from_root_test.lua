local zwave = require("zwave")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local storage_helper = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils" )

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
local binary_sensor_command_class = {
    id = class_ids.sensor_binary,
    sensor_types = {
        8,
        10
    }
}

local notification_command_class = {
    id = class_ids.notification,
    notifications = {
        {
            event_types = {
                "dw_is_opened",
                "dw_is_closed"
            },
            type = "access_control",
            unknown_event_types = {}
        },
        {
            event_types = {
                "product_cover_removed"
            },
            type = "home_security",
            unknown_event_types = {}
        }
    },
    v1_supported = false,
    version = 4
}

local channel_0 = {
    id = 0,
    classes = {
        notification_command_class,
        binary_sensor_command_class,
    }
}

local node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
    is_battery_powered = true,

    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",

    security = { "s0" },
    security_class_started = { "s0" },

    channels = { channel_0 }
}
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000 
} )

-- check adding result
local expected_items = {
    {
        name = item_names.dw_state,
        tag = item_tags.access_control,
        cc_id = class_ids.notification,
        channel_id = 0,
    },
    {
        name = item_names.dw_state,
        cc_id = class_ids.sensor_binary,
        tag = item_tags.access_control,
        channel_id = 0,
        is_merged = true
    },
    {
        name = item_names.security_threat,
        tag = "access_control.security_tag",
        cc_id = class_ids.notification,
        channel_id = 0,
        main_item_name = item_names.dw_state
    },
    {
        name = item_names.tampering_cover_alarm,
        tag = item_tags.home_security,
        cc_id = class_ids.notification,
        channel_id = 0,
    },
    {
        name = item_names.tampering_cover_alarm,
        tag = item_tags.home_security,
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        is_merged = true
    },
}

local expected_devices = {
    {
        name = "Door/Window Sensor",
        category = "security_sensor",
        subcategory = "door",
        items = expected_items,
        node_id = "9000",
        battery_powered = true
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

-- modify zwave node
zwave.nodes = {}
node.channels[1].classes = { binary_sensor_command_class }
zwave.add_node( node )

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

expected_devices[1].items = {
    {
        name = item_names.dw_state,
        cc_id = class_ids.sensor_binary,
        tag = item_tags.access_control,
        channel_id = 0,
    },
    {
        name = item_names.security_threat,
        tag = "access_control.security_tag",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        main_item_name = item_names.dw_state
    },
    {
        name = item_names.tampering_cover_alarm,
        tag = item_tags.home_security,
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
    },
}
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )
