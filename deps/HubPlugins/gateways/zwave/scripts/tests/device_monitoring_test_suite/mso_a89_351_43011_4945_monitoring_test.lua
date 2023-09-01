local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require("zwave")
local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local item_names = require( "HUB:zwave/scripts/resources/item_names" )

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

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

----------------------------------------------------------------------------------
-- send motion sensor event
events_handler( {
    event = "multilevel_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.multilevel_sensor,
    sensor_type = "general_purpose",
    value = 1,
    scale = "float"
} )

local function find_item( item_name, item_tag )
    return core.get_item( storage:call("find_item_id", item_name, 9000, class_ids.multilevel_sensor, 0, item_tag  ) )
end

local motion_item = find_item( item_names.motion, "access_control" )
local security_motion_item = find_item( item_names.motion, "access_control.security_tag" )

tests.assert_eq( motion_item.value, true, "check motion value after binary sensor value changing" )
tests.assert_eq( security_motion_item.value, true, "check motion security value after binary sensor value changing" )

---------------------------------------------------------------------------------
events_handler( {
    event = "multilevel_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.multilevel_sensor,
    sensor_type = "air_temperature",
    value = 2.0,
    scale = "celsius"
} )
local temp_item = find_item(item_names.temp)
tests.assert_eq(  temp_item.value, {value = 2.0, scale = "celsius"}, "check motion value after binary sensor value changing" )

----------------------------------------------------------------------------------
events_handler( {
    event = "multilevel_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.multilevel_sensor,
    sensor_type = "illuminance",
    value = 3.0,
    scale = "lux"
} )
local illuminance_item = find_item(item_names.lux)
tests.assert_eq(  illuminance_item.value, {value = 3.0, scale = "lux"}, "check motion value after binary sensor value changing" )

----------------------------------------------------------------------------------                       
events_handler( {
    event = "multilevel_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.multilevel_sensor,
    sensor_type = "humidity",
    value = 4.0,
    scale = "percent"
} )
local humidity_item = find_item(item_names.humidity)
tests.assert_eq(  humidity_item.value, {value = 4.0, scale = "percent"}, "check motion value after binary sensor value changing" )                    
                        
----------------------------------------------------------------------------------
events_handler( {
    event = "multilevel_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.multilevel_sensor,
    sensor_type = "co2_level",
    value = 5.0,
    scale = "parts_per_million"
} )
local co2_level_item = find_item(item_names.co2_level)
tests.assert_eq(  co2_level_item.value, {value = 5.0, scale = "parts_per_million"}, "check motion value after binary sensor value changing" )

----------------------------------------------------------------------------------
events_handler( {
    event = "multilevel_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.multilevel_sensor,
    sensor_type = "loudness",
    value = 6.0,
    scale = "a_weighted_decibels"
} )
local loudness_item = find_item(item_names.loudness)
tests.assert_eq(  loudness_item.value, {value = 6.0, scale = "a_weighted_decibels"}, "check motion value after binary sensor value changing" )

----------------------------------------------------------------------------------
events_handler( {
    event = "multilevel_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.multilevel_sensor,
    sensor_type = "particulate_matter_2_dot_5",
    value = 7.0,
    scale = "micro_gram_per_cubic_meter"
} )
local particulate_matter_2_dot_5_item = find_item(item_names.particulate_matter_2_dot_5)
tests.assert_eq(  particulate_matter_2_dot_5_item.value, {value = 7.0, scale = "micro_gram_per_cubic_meter"}, "check motion value after binary sensor value changing" )

----------------------------------------------------------------------------------
events_handler( {
    event = "multilevel_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.multilevel_sensor,
    sensor_type = "volatile_organic_compound_level",
    value = 8.0,
    scale = "parts_per_million"
} )
local volatile_organic_compound_level_item = find_item(item_names.volatile_organic_compound_level)
tests.assert_eq(  volatile_organic_compound_level_item.value, {value = 8.0, scale = "parts_per_million"}, "check motion value after binary sensor value changing" )
----------------------------------------------------------------------------------            
events_handler( {
    event = "multilevel_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.multilevel_sensor,
    sensor_type = "smoke_density",
    value = 9.0,
    scale = "percent"
} )
local smoke_density_item = find_item(item_names.smoke_density)
tests.assert_eq(  smoke_density_item.value, {value = 9.0, scale = "percent"}, "check motion value after binary sensor value changing" )
