local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )

zwave.request_node_values = tests.create_mock()

local function test_impl( multilevel_sensor_humidity_cc )

    local channel_0 = { 
        id = 0,
        classes = { 
            { id = 129 },
            { id = 135 },
            { id = 114 },
            { id = class_ids.basic },
            {
                id = class_ids.multilevel_sensor,
                sensors = {{
                    sensor_type = "air_temperature",
                    scales = { "celsius" }
                }}
            },
            { id = class_ids.thermostat_mode,
                modes = { 0,1,2,3 }
            },
            { id = class_ids.thermostat_operating_state },
            { id = 134 },
            { id = 112 },
            { id = class_ids.battery },
            { id = 133 },
            { id = 96 },
            { id = class_ids.thermostat_setpoint,
                setpoints = {
                    {type = 1},
                    {type = 2},
                    {type = 11},
                    {type = 12}
                }
            },
            { id = class_ids.thermostat_fan_mode,
                modes = { 0,1 }
            },
        }
    }

    local channel_1 = {
        id = 1,
        classes = {
            { id = class_ids.switch_binary } -- some random cc that shouldn't be processed in channel 1
        }
    }

    local channel_2 = {
        id = 2,
        classes = {
            multilevel_sensor_humidity_cc,
            { id = 133 }
        }
    }

    local node = {
        id = "9000",
        manufacturer_id = 152,
        product_type = 25601,
        product_id = 263,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0, channel_1, channel_2 }
    }

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )

    -- check adding result
    local expected_thermostat_items = {
        {
            name = "basic",
            cc_id = class_ids.basic,
            channel_id = 0,
        },
        {
            name = "temp",
            cc_id = class_ids.multilevel_sensor,
            channel_id = 0,
        },
        {
            name = "battery",
            cc_id = class_ids.battery,
            channel_id = 0,
        },
        {
            name = "thermostat_mode",
            cc_id = class_ids.thermostat_mode,
            channel_id = 0,
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
        },
        {
            name = "thermostat_operating_state",
            cc_id = class_ids.thermostat_operating_state,
            channel_id = 0,
        },
        {
            name = "thermostat_fan_mode",
            cc_id = class_ids.thermostat_fan_mode,
            channel_id = 0,
        }
    }

    local expected_humidity_items = {
        {
            name = "humidity",
            cc_id = class_ids.multilevel_sensor,
            channel_id = 2
        }
    }

    local expected_devices = {
        {
            name = "Thermostat",
            category = "hvac",
            subcategory = "hvac",
            items = expected_thermostat_items,
            node_id = "9000",
            battery_powered = true,
            subdevices = {
                {
                    name = "Humidity Sensor",
                    category = "humidity",
                    subcategory = "",
                    items = expected_humidity_items
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

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = 152,
        product_type = 25601,
        product_id = 263
    } )

    -- check node removing result
    loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )

end

tests.run_with_params( test_impl,
{
    {
        value = { id = class_ids.multilevel_sensor },
        description = "sensor multilevel without sensors in second channel"
    },
    {
        value =  {
            id = class_ids.multilevel_sensor,
            sensors = {{
                    sensor_type = "humidity",
                    scales = { "percent" },
                }}
        },
        description = "sensor multilevel with humidity sensor in second channel"
    }
} )
