local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()


local cc_binary_sensor = {
    id = class_ids.sensor_binary,
    sensor_types = {
        0x01, -- General purpose
    }
}

local cc_basic = {
    id = class_ids.basic,
}

local channel_0 = {
    id = 0,
    classes = { cc_binary_sensor, cc_basic,  { id = class_ids.battery } }
}

local node = {
    id = "9000",
    manufacturer_id = 282,
    product_type = 1537,
    product_id = 2305,
    is_battery_powered = true,

    specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_BINARY",
    generic_type = "GENERIC_TYPE_SENSOR_BINARY",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

function zwave:get_node_info( node_id )
    return node
end

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local expected_motion_items = {
    {
        name = "motion",
        cc_id = class_ids.sensor_binary,
        tag = "home_security",
        channel_id = 0,
    },
    {
        name = "basic",
        cc_id = class_ids.basic,
        channel_id = 0,
    },
    {
        name = "battery",
        cc_id = class_ids.battery,
        channel_id = 0,
    },
    {
        name = "security_threat",
        tag = "home_security.security_tag",
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        main_item_name = "motion"
    }
}


local expected_devices = {
    {
        name = "Motion Sensor",
        category = "security_sensor",
        subcategory = "motion",
        items = expected_motion_items,
        node_id = "9000",
        battery_powered = true,
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

events_handler( {
    event = "value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.basic,
    value = 0xFF
} )

-- send motion basic event
local function find_item( item_name, item_tag )
    return core.get_item( storage:call("find_item_id", item_name, 9000, class_ids.sensor_binary, 0, item_tag  ) )
end

local motion_item = find_item( "motion", "home_security" )
local security_motion_item = find_item( "motion", "home_security.security_tag" )

tests.assert_eq( motion_item.value, true, "check motion value after basic value changing" )
tests.assert_eq( security_motion_item.value, true, "check motion security value after basic value changing" )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 282,
    product_type = 1537,
    product_id = 2305,
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
