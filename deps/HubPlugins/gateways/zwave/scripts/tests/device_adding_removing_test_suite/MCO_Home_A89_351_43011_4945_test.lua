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
                    firmware_versions = { "6.3" },
                    hardware_version = 255,
                    id = class_ids.firmware_update,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "6.8"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            }
                        }
                    },
                    association_groups_count = 1,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    id = class_ids.configuration,
                    parameters = {},
                    version = 1
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
                        },
                        {
                            scales = { "float" },
                            sensor_type = "general_purpose"
                        },
                        {
                            scales = { "lux" },
                            sensor_type = "illuminance"
                        },
                        {
                            scales = { "percent" },
                            sensor_type = "humidity"
                        },
                        {
                            scales = { "parts_per_million" },
                            sensor_type = "co2_level"
                        },
                        {
                            scales = { "a_weighted_decibels" },
                            sensor_type = "loudness"
                        },
                        {
                            scales = { "micro_gram_per_cubic_meter" },
                            sensor_type = "particulate_matter_2_dot_5"
                        },
                        {
                            scales = { "parts_per_million" },
                            sensor_type = "volatile_organic_compound_level"
                        },
                        {
                            scales = { "percent" },
                            sensor_type = "smoke_density"
                        }
                    },
                    version = 10
                },
                {
                    firmware_ids = { 4945 },
                    id = class_ids.version,
                    version = 4
                },
                {
                    id = class_ids.basic,
                    version = 0
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_UNKNOWN",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_smart_include = false,
    manufacturer_id = 351,
    product_id = 4945,
    product_type = 43011,
    security = {
        "unauthenticated"
    },
    security_class_started = {
        "unauthenticated"
    },
    specific_type = "SPECIFIC_TYPE_UNKNOWN"
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
        name = "9-in-1 sensor",
        category = "security_sensor",
        subcategory = "motion",
        items = { 
            {
                name = item_names.basic,
                cc_id = class_ids.basic,
                channel_id = 0
            },
            {
                name = item_names.motion,
                cc_id = class_ids.multilevel_sensor,
                channel_id = 0,
                tag = "access_control",
            },
            {
                name = "security_threat",
                tag = "access_control.security_tag",
                cc_id = class_ids.multilevel_sensor,
                channel_id = 0,
                main_item_name = "motion"
            }
        },
        subdevices = {
            {
                name = "Loudness Sensor",
                category = "level_sensor",
                subcategory = "sound",
                items = {
                    {
                        name = item_names.loudness,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                }
            },
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
            },
            {
                name = "CO2 Level Sensor",
                category = "level_sensor",
                subcategory = "co2",
                items = {
                    {
                        name = item_names.co2_level,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                }
            },
            {
                name = "Smoke Density Sensor",
                category = "level_sensor",
                subcategory = "smoke",
                items = {
                    {
                        name = item_names.smoke_density,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                }
            },
            {
                name = "Volatile Organic Compound Level Sensor",
                category = "level_sensor",
                subcategory = "air_pollution",
                items = {
                    {
                        name = item_names.volatile_organic_compound_level,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                }
            },
            {
                name = "Particulate Matter 2.5 Sensor",
                category = "level_sensor",
                subcategory = "particulate_matter",
                items = {
                    {
                        name = item_names.particulate_matter_2_dot_5,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                }
            },
            {
                name = "Light Level Sensor",
                category = "light_sensor",
                subcategory = "",
                items = {
                    {
                        name = item_names.lux,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                }
            },
           {
                name = "Humidity Sensor",
                category = "humidity",
                subcategory = "",
                items = {
                    {
                        name = item_names.humidity,
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
    manufacturer_id = 351,
    product_id = 4945,
    product_type = 43011
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
