local tests = require( "tests" )
local storage = require( "storage" )
local core = require( "core" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local cc_binary_sensor = {
    id = class_ids.sensor_binary,
    sensor_types = {
        0x03, -- co_alarm
        0x05, -- heat_alarm
    }
}

local channel_0 = {
    id = 0,
    classes = { cc_binary_sensor }
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

local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

local zwave = require( "zwave" )
zwave.request_node_values = tests.create_mock()
zwave.start_exclude = tests.create_mock()

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- check adding result
local expected_co_items = {
    {
        name = "co_alarm",
        tag = "co_alarm",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        value_type = "token",
        enum = {
            "unknown",
            "no_co",
            "co_detected",
        }
    },
    {
        name = "security_threat",
        tag = "co_alarm.security_tag",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        main_item_name = "co_alarm"
    }
}

local expected_heat_items = {
    {
        name = "heat_alarm",
        tag = "heat_alarm",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        value_type = "token",
        enum = {
            "unknown",
            "heat_ok",
            "overheat_detected",
        }
    },
    {
        name = "security_threat",
        tag = "heat_alarm.security_tag",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        main_item_name = "heat_alarm",
    }
}


local expected_devices = {
    {
        name = "CO Sensor",
        category = "security_sensor",
        subcategory = "co",
        items = expected_co_items,
        node_id = "9000",
        battery_powered = true,

        subdevices = {
            {
                name = "Heat Sensor",
                category = "security_sensor",
                subcategory = "heat",
                items = expected_heat_items
            }
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- change mapping
local device_id = assert( storage.get_string( "9000" ) )
storage.set_string( "9000", "another_device_id" )

-- try to remove
loadfile( "HUB:zwave/scripts/start_exclude" )( {deviceId = device_id} )

-- check node removing result
check_context.skip_node_id_to_device_check = true
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )

tests.assert_called( zwave.start_exclude, "check no start_exclude called", {} )
