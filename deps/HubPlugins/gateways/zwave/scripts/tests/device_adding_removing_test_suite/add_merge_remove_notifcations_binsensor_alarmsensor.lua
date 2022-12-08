local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local cc_binary_sensor = { 
    id = class_ids.sensor_binary,
    sensor_types = { 
        0x04, -- co2_alarm
        0x06, -- water_leak_alarm
    }
}

local cc_notification = { 
    id = class_ids.notification,
    notifications = { 
        {
            type = "co2_alarm",
            event_types = { "co2_detected" }
        },
        {
            type = "water_alarm",
            event_types = { "water_leak_detected" }
        }
    }
}

local cc_alarm_sensor = { 
    id = class_ids.alarm_sensor,
    sensor_types = { "water_alarm", "co_alarm" }
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


-- check node adding result
local expected_water_items = {
    {
        name = "water_leak_alarm",
        tag = "water_alarm",
        cc_id = class_ids.sensor_binary,
        channel_id = 0
    },
    {
        name = "security_threat",
        tag = "water_alarm.security_tag",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        main_item_name = "water_leak_alarm"
    },
    {
        name = "water_leak_alarm",
        tag = "water_alarm",
        cc_id = class_ids.notification,
        channel_id = 0,
        is_merged = true
    },
    {
        name = "water_leak_alarm",
        tag = "water_alarm",
        cc_id = class_ids.notification,
        channel_id = 0,
        is_merged = true
    }
}

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
        cc_id = class_ids.notification,
        channel_id = 0,
        is_merged = true
    }
}

local expected_co_items = {
    {
        name = "co_alarm",
        tag = "co_alarm",
        cc_id = class_ids.alarm_sensor,
        channel_id = 0
    },
    {
        name = "security_threat",
        tag = "co_alarm.security_tag",
        cc_id = class_ids.alarm_sensor,
        channel_id = 0,
        main_item_name = "co_alarm"
    }
}


local expected_devices = {
    {
        name = "Water Leak Sensor",
        category = "security_sensor",
        subcategory = "leak",
        items = expected_water_items,
        node_id = "100500",
        battery_powered = true,

        subdevices = {
            {
                name = "CO2 Sensor",
                category = "security_sensor",
                subcategory = "co2",
                items = expected_co2_items
            },
            {
                name = "CO Sensor",
                category = "security_sensor",
                subcategory = "co",
                items = expected_co_items
            },  
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    tests = tests
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )


-- remove node
events_handler( {
    event = "node_removed",
    node_id = "100500",     
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2, 
} )

-- check removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
