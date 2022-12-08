local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )

zwave.request_node_values = tests.create_mock()

local channel_0 = { 
    id = 0,
    classes = { 
        { id = class_ids.thermostat_mode,
            modes = { 0,1,2,7,13 }
        },
        { id = class_ids.thermostat_setpoint,
            setpoints = {
                {type = 1},
                {type = 2},
                {type = 9},
                {type = 15}
            }
        }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- check adding result

local expected_devices = {
    {
        name = "Thermostat",
        category = "hvac",
        subcategory = "hvac",
        items = {
            {
                name = "thermostat_mode",
                cc_id = class_ids.thermostat_mode,
                channel_id = 0,
                value_type = "token",
                enum = {
                    "off",
                    "heat",
                    "cool",
                    "furnace",
                    "away"
                }
            },
            {
                name = "thermostat_setpoint",
                cc_id = class_ids.thermostat_setpoint,
                channel_id = 0,
            },
            {
                name = "thermostat_setpoint_heating",
                cc_id = class_ids.thermostat_setpoint,
                channel_id = 0,
            },
            {
                name = "thermostat_setpoint_cooling",
                cc_id = class_ids.thermostat_setpoint,
                channel_id = 0,
            },
            {
                name = "thermostat_energy_saving_mode",
                cc_id = class_ids.thermostat_setpoint,
                channel_id = 0,
                value_type = "token",
                enum = {
                    "off",
                    "low",
                    "moderate",
                    "aggressive",
                    "low_postponed",
                    "moderate_postponed",
                    "aggressive_postponed"
                },
                value = "off"
            }
        },
        settings = {
            {
                value_type = "action",
                value = {
                    text = "Rediscover",
                    lang_tag = "rediscover_tag"
                },
                tag = "rediscover"
            },
            {
                label = {
                    text = "Controlled thermostat setpoint type",
                    lang_tag = "controlled_thermostat_setpoint_type"
                },
                description = {
                    text = "Controlled thermostat setpoint type",
                    lang_tag = "controlled_thermostat_setpoint_type"
                },
                value_type = "token",
                enum = {
                    cooling = { 
                        lang_tag = "thermostat_setpoint_type_cooling", 
                        text = "Cooling" 
                    },
                    default = {
                        lang_tag = "thermostat_setpoint_type_mode_dependent",
                        text = "Mode dependent (default)"
                    },
                    full_power = {
                        lang_tag = "thermostat_setpoint_type_full_power",
                        text = "Full power"
                    },
                    heating = {
                        lang_tag = "thermostat_setpoint_type_heating",
                        text = "Heating"
                    },
                    moist_air = {
                        lang_tag = "thermostat_setpoint_type_moist_air",
                        text = "Moist air"
                    }   
                },
                value = "default",
                value_default = "default",
                tag = "controlled_setpoint_type",
                cc_id = class_ids.thermostat_setpoint
            }
        },
        node_id = "9000",
        battery_powered = true
    }
}

local check_context = {
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
