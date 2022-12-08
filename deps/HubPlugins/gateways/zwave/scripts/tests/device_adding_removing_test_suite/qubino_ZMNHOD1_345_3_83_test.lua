local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()


local node = {
    generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
    icon_type = "ICON_TYPE_GENERIC_WINDOW_COVERING_POSITION_ENDPOINT_AWARE",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 345,
    product_id = 83,
    product_type = 3,
    security = {
        "none"
    },
    security_class_started = {
        "none"
    },
    specific_type = "SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL",
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 112,
                    parameters = {},
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    firmware_versions = {
                        1.1
                    },
                    hardware_version = 1,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 38,
                    version = 3
                },
                {
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 4
                },
                {
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "celsius"
                            },
                            sensor_type = "air_temperature"
                        }
                    },
                    version = 7
                },
                {
                    id = 96,
                    version = 4
                },
                {
                    id = 133,
                    version = 2
                },
                {

                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                }
            },
            id = 0
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    firmware_versions = {
                        "1.1"
                    },
                    hardware_version = 1,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 38,
                    version = 3
                },
                {
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 4
                },
                {
                    id = 133,
                    version = 2
                },
                {
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                }
            },
            id = 1
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    firmware_versions = {
                        "1.1"
                    },
                    hardware_version = 1,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
                },
                {
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "celsius"
                            },
                            sensor_type = "air_temperature"
                        }
                    },
                    version = 7
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                }
            },
            id = 2
        }
    }
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
        name = "Flush Shutter DC",
        category = "window_cov",
        subcategory = "window_cov",
        items = { 
            {
                name = "dimmer",
                cc_id = class_ids.dimmer,
                channel_id = 0,
            },
            {
                name = "dimmer",
                cc_id = class_ids.dimmer,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "dimmer_up",
                cc_id = class_ids.dimmer,
                channel_id = 0,
            },
            {
                name = "dimmer_up",
                cc_id = class_ids.dimmer,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "dimmer_down",
                cc_id = class_ids.dimmer,
                channel_id = 0,
            },
            {
                name = "dimmer_down",
                cc_id = class_ids.dimmer,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "dimmer_stop",
                cc_id = class_ids.dimmer,
                channel_id = 0,
            },
            {
                name = "dimmer_stop",
                cc_id = class_ids.dimmer,
                channel_id = 1,
                is_merged = true
            },
            {
              name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            },
            {
              name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "meter_reset",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "meter_reset",
                cc_id = class_ids.meter,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "electric_meter_kwh",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "electric_meter_kwh",
                cc_id = class_ids.meter,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "electric_meter_watt",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "electric_meter_watt",
                cc_id = class_ids.meter,
                channel_id = 1,
                is_merged = true
            },
            {
                name = "goto_favorite",
                cc_id = class_ids.dimmer,
                tag = "dimmer",
                channel_id = 0  
            },
            {
                name = "goto_favorite",
                cc_id = class_ids.dimmer,
                tag = "dimmer",
                channel_id = 1,
                is_merged = true
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
                    },
                    {
                        name = item_names.temp,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 2,
                        is_merged = true
                   }                
                },
            }
        },
        node_id = "9000",
        battery_powered = false,
        security = "no",
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
