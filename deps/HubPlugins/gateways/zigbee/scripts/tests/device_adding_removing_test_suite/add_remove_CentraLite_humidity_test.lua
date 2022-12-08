local tests = require( "tests" )
local zigbee = require( "zigbee" )

local function test_impl( model )
    local cluster_meter = {
        name = "temperature_measurement",
        attributes = {
            {
                name = "MeasuredValue",
                formatted_value = 0
            },
            {
                name = "MinMeasuredValue",
                formatted_value = 0
            },
            {
                name = "MaxMeasuredValue",
                formatted_value = 0
            }
        }
    }

    local cluster_humidity = {
        id = 0xFC45,
        name = "unknown",
        attributes = {
            {
                id = 0x00,
                name = "unknown",
                formatted_value = "0"
            },
            {
                id = 0x01,
                name = "unknown",
                formatted_value = "0"
            },
            {
                id = 0x02,
                name = "unknown",
                formatted_value = "0"
            }
        }
    }

    local endpoint_1 = {
        id = 1,
        clusters = { cluster_meter, cluster_humidity }
    }

    local node = {
        id = 42,
        manufacturer_id = "CentraLite",
        model_id = model,
        security = 0,
        is_reachable = 1,
        is_battery_powered = true,

        endpoints = { endpoint_1 }
    }

    local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

    local zigbee = require( "zigbee" )

    -- prepare zigbee
    zigbee.add_node( node )

    zigbee.start_mfg_specific_interview = tests.create_mock()
    -- add node
    events_handler( {
        event = "node_added",
        node_id = 42
    } )

    -- check adding result
    local expected_temp_items = {
        {
            name = "temp",
            endpoint_id = 1,
            cluster_name = "temperature_measurement",
            attribute_name = "MeasuredValue",
            value_type = "temperature",
            value = { value = 0, scale = "celsius" }
        }
    }

    local expected_temperature_settings = {
        {
            name = "temperature_calibration",
            endpoint_id = 1,
            cluster_name = "temperature_measurement",
            attribute_name = "MeasuredValue",
            label = {
                lang_tag = "zigbee_device_setting_temperature_calibration_label",
                text = "Zigbee device temperature calibration"
            },
            description = {
                lang_tag = "zigbee_device_setting_temperature_calibration_description",
                text = "Value to calibrate sensor temperature"
            },
            value_type = "temperature",
            value = {
                value = 0,
                scale = "celsius"
            },
            value_default = {
                value = 0,
                scale = "celsius"
            },
            value_min = {
                value = -2.5,
                scale = "celsius"
            },
            value_max = {
                value = 2.5,
                scale = "celsius"
            }
        }
    }

    local expected_humidity_items = {
        {
            name = "humidity",
            endpoint_id = 1,
            cluster_name = 0xFC45,
            attribute_name = 0x00,
            tag = ".mfg_specific",
            value_type = "humidity",
            value = { value = 0, scale = "percent" }
        }
    }


    local expected_devices = {
        {
            name = "Humidity Sensor",
            category = "humidity",
            subcategory = "",
            items = expected_humidity_items,
            node_id = 42,
            battery_powered = true,

            subdevices = {
                {
                    name = "Temperature Sensor",
                    category = "temperature",
                    subcategory = "",
                    items = expected_temp_items,
                    settings = expected_temperature_settings
                }
            }
        }
    }

    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {},
        setting_ids = {}
    }

    local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"

    loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices, check_context )

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = 42,
        manufacturer_id = "CentraLite",
        model_id = model
    } )

    -- check node removing result
    loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = "3310-G",
            description = "test with 3310-G model"
        },
        {
            value = "3310-Geu",
            description = "test with 3310-Geu model"
        }
    }
)
