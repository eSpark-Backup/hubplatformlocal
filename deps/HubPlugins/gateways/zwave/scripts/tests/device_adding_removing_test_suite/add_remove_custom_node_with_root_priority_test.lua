local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local device_type_id = loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(316, 2, 31)
tests.assert_ne( loadfile( "HUB:zwave/scripts/model/devices/devices_info/generated/" .. device_type_id ),
                 nil, 
                 "device " .. device_type_id .. " not found" )

local cc_binary_sensor = { 
    id = class_ids.sensor_binary,
    sensor_types = { 
        0x02, -- smoke_alarm
        0x0C, -- motion
    }
}

local channel_0 = { 
    id = 0,
    classes = { cc_binary_sensor }
}


local node = {
    id = "42",
    manufacturer_id = 316,
    product_type = 2,
    product_id = 31,
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
    node_id = 42 
} )

-- check adding result
local expected_motion_items = {
    {
        name = "motion",
        tag = "home_security",
        cc_id = class_ids.sensor_binary,
        channel_id = 0
    },
    {
        name = "security_threat",
        tag = "home_security.security_tag",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        main_item_name = "motion"
    }
}

local expected_smoke_items = {
    {
        name = "smoke_alarm",
        tag = "smoke_alarm",
        cc_id = class_ids.sensor_binary,
        channel_id = 0
    },
    {
        name = "security_threat",
        tag = "smoke_alarm.security_tag",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        main_item_name = "smoke_alarm"
    }
}


local expected_devices = {
    {
        name = "Flood Sensor",
        category = "security_sensor",
        subcategory = "motion",
        items = expected_motion_items,
        node_id = "42",
        battery_powered = true,

        subdevices = {
            {
                name = "Smoke Sensor",
                category = "security_sensor",
                subcategory = "smoke",
                items = expected_smoke_items
            }
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "42",     
    manufacturer_id = 316,
    product_type = 2,
    product_id = 31, 
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
