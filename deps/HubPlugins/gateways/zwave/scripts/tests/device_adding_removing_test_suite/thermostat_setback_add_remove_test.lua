local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()


local channel_0 = {
    id = 0,
    classes = { 
        {
            id = class_ids.thermostat_setback,
            version = 1
        }
    }
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

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- check adding result

local expected_settings = {
    {
        value_type = "temperature",
        value = {
            value = 0,
            scale = "celsius",
            relative = true
        },
        value_min = {
            value = -12.8,
            scale = "celsius",
            relative = true  
        },
        value_max = {
            value = 12.0,
            scale = "celsius",
            relative = true  
        },
        cc_id = class_ids.thermostat_setback,
        tag = "thermostat_setback_value"
    },
    {
        value_type = "token",
        value = "unknown",
        enum = {
            unknown = { 
                text = "Unknown"
            },
            custom_value = {
                text = "Custom Value"
            },
            frost_protection = {
                text = "Frost Protection"
            },
            energy_saving_mode = {
                text = "Energy Saving Mode"
            }
        },
        value_default = "custom_value",
        cc_id = class_ids.thermostat_setback,
        tag = "thermostat_setback_mode"
    },
    {
        value_type = "token",
        value = "unknown",
        value_default = "permanent_override",
        enum = {
            unknown = { 
                text = "Unknown"
            },
            no_override = {
                text = "No Override"
            },
            temporary_override = {
                text = "Temporary Override"
            },
            permanent_override = {
                text = "Permanent Override"
            }
        },
        cc_id = class_ids.thermostat_setback,
        tag = "thermostat_setback_override_type"
    },
    {
        value_type = "action",
        value = {
            text = "Rediscover",
            lang_tag = "rediscover_tag"
        },
        tag = "rediscover"
    }
}

local expected_devices = {
    {
        name = tests.skipped_param,
        category = tests.skipped_param,
        items = { },
        settings = expected_settings,
        node_id = "9000",
        battery_powered = true
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )

-- check removing result
hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context ) 
