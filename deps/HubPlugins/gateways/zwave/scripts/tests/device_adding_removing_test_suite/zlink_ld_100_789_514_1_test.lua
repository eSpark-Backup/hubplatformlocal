local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()


local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups = {},
                    association_groups_count = 2,
                    id = 133,
                    version = 2
                },
                {
                    id = class_ids.sensor_binary,
                    sensor_types = {},
                    version = 1
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    application_build_number = 52445,
                    application_framework_api_build_number = 52445,
                    application_framework_api_version = "4.1.1",
                    application_version = "4.1.1",
                    firmware_versions = { "50.7" },
                    hardware_version = 255,
                    host_interface_build_number = 0,
                    id = class_ids.version,
                    library_type = "enhanced_slave",
                    protocol_build_number = 86,
                    protocol_version = "6.2.0",
                    sdk_version = "6.81.1",
                    version = 3,
                    zwave_protocol_version = "6.2"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = class_ids.notification,
                    notifications = {
                        {
                            event_types = {
                                "water_leak_detected"
                            },
                            type = "water_alarm",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = class_ids.configuration,
                    version = 3
                },
                {
                    id = class_ids.multilevel_sensor,
                    sensors = {
                        {
                            scales = {
                                "celsius",
                                "fahrenheit"
                            },
                            sensor_type = "air_temperature"
                        }
                    },
                    version = 11
                },
                {
                    id = class_ids.switch_binary,
                    version = 1
                },
                {
                    firmware_ids = { 513 },
                    id = class_ids.firmware_update,
                    version = 4
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
    id = "9000",
    is_battery_powered = false,
    is_reachable = true,
    is_smart_include = false,
    manufacturer_id = 789,
    product_id = 1,
    product_type = 514,
    security = { "authenticated" },
    security_class_started = { "authenticated" },
    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR"
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )


-- check adding result
local expected_devices = {
    {
        name = "Water Leak Sensor",
        category = "security_sensor",
        subcategory = "leak",
        items = { 
            {
                name = item_names.water_leak_alarm,
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "water_alarm"
            },
            {
                name = item_names.security_threat,
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "water_alarm.security_tag",
                main_item_name = item_names.water_leak_alarm
            },
            {
                name = item_names.switch,
                cc_id = class_ids.switch_binary,
                channel_id = 0
            },
        },
        subdevices = {
            {
                name = "Temperature Sensor",
                category = "temperature",
                subcategory = "",
                items = {
                    {
                        name = item_names.temp,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                }
            }
        },
        node_id = "9000",
        battery_powered = false,
        security = "authenticated",
    }
}

local check_context = {}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 789,
    product_type = 514,
    product_id = 1,
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
