local tests = require( "tests" )
local zigbee = require( "zigbee" )

local function test_impl( model )
    local node = {
        endpoints = {
            {
                clusters = {
                    {
                        attributes = {
                            {
                                cluster_id = 0,
                                formatted_value = "CentraLite",
                                id = 4,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "ManufacturerName",
                                type = "string",
                                value = "CentraLite"
                            },
                            {
                                cluster_id = 0,
                                formatted_value = "3305-Geu",
                                id = 5,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "ModelIdentifier",
                                type = "string",
                                value = "3305-Geu"
                            }
                        },
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 1,
                        id = 0,
                        name = "basic"
                    },
                    {
                        attributes = {
                            {
                                cluster_id = 1,
                                formatted_value = 23,
                                id = 32,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "BatteryVoltage",
                                type = "uint8_t",
                                value = "23"
                            },
                            {
                                cluster_id = 1,
                                formatted_value = "other",
                                id = 49,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "BatterySize",
                                type = "enum8_t",
                                value = "2"
                            },
                            {
                                cluster_id = 1,
                                formatted_value = 1,
                                id = 51,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "BatteryQuantity",
                                type = "uint8_t",
                                value = "1"
                            }
                        },
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 1,
                        id = 1,
                        name = "power_configuration"
                    },
                    {
                        attributes = {},
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 1,
                        id = 3,
                        name = "identify"
                    },
                    {
                        attributes = {},
                        commands = {},
                        data_holder = "client",
                        endpoint_id = 1,
                        id = 25,
                        name = "ota_upgrade"
                    },
                    {
                        attributes = {
                            {
                                cluster_id = 32,
                                formatted_value = "14400",
                                id = 0,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "CheckInInterval",
                                type = "uint32_t",
                                value = "14400"
                            }
                        },
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 1,
                        id = 32,
                        name = "poll_control"
                    },
                    {
                        attributes = {
                            {
                                cluster_id = 1026,
                                formatted_value = 2699,
                                id = 0,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "MeasuredValue",
                                type = "int16_t",
                                value = "2699"
                            },
                            {
                                cluster_id = 1026,
                                formatted_value = -4000,
                                id = 1,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "MinMeasuredValue",
                                type = "int16_t",
                                value = "-4000"
                            },
                            {
                                cluster_id = 1026,
                                formatted_value = 6000,
                                id = 2,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "MaxMeasuredValue",
                                type = "int16_t",
                                value = "6000"
                            },
                            {
                                cluster_id = 1026,
                                formatted_value = 150,
                                id = 3,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "Tolerance",
                                type = "uint16_t",
                                value = "150"
                            }
                        },
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 1,
                        id = 1026,
                        name = "temperature_measurement"
                    },
                    {
                        attributes = {
                            {
                                cluster_id = 1280,
                                formatted_value = {
                                    alarm_1 = "intrusion_indication",
                                    alarm_2 = "presence_indication",
                                    zone_type = "motion_sensor"
                                },
                                id = 1,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "ZoneType",
                                type = "enum16_t",
                                value = "13"
                            },
                            {
                                cluster_id = 1280,
                                formatted_value = {
                                    ac = "ok",
                                    alarm_1 = "off",
                                    alarm_2 = "on",
                                    battery = "ok",
                                    restore_reports = "yes",
                                    supervision_reports = "no",
                                    tamper = "tampered",
                                    trouble = "ok"
                                },
                                id = 2,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "ZoneStatus",
                                type = "bitmap16_t",
                                value = "38"
                            }
                        },
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 1,
                        id = 1280,
                        name = "ias_zone"
                    },
                    {
                        attributes = {},
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 1,
                        id = 2821,
                        name = "diagnostics"
                    }
                },
                device_id = 1026,
                id = 1,
                profile_id = 260
            },
            {
                clusters = {
                    {
                        attributes = {},
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 2,
                        id = 0,
                        name = "basic"
                    },
                    {
                        attributes = {},
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 2,
                        id = 1,
                        name = "power_configuration"
                    },
                    {
                        attributes = {},
                        commands = {},
                        data_holder = "clientServer",
                        endpoint_id = 2,
                        id = 3,
                        name = "identify"
                    },
                    {
                        attributes = {
                            {
                                cluster_id = 1030,
                                formatted_value = "1",
                                id = 0,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "Occupancy",
                                type = "bitmap8_t",
                                value = "1"
                            },
                            {
                                cluster_id = 1030,
                                formatted_value = "0",
                                id = 1,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "OccupancySensorType",
                                type = "enum8_t",
                                value = "0"
                            },
                            {
                                cluster_id = 1030,
                                formatted_value = "30",
                                id = 16,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "PIROccupiedToUnoccupiedDelay",
                                type = "uint16_t",
                                value = "30"
                            },
                            {
                                cluster_id = 1030,
                                formatted_value = "0",
                                id = 17,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "PIRUnoccupiedToOccupiedDelay",
                                type = "uint16_t",
                                value = "0"
                            },
                            {
                                cluster_id = 1030,
                                formatted_value = "1",
                                id = 18,
                                is_mfg_specific = false,
                                mfg_code = 65535,
                                name = "PIRUnoccupiedToOccupiedThreshold",
                                type = "uint8_t",
                                value = "1"
                            }
                        },
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 2,
                        id = 1030,
                        name = "occupancy_sensing"
                    },
                    {
                        attributes = {},
                        commands = {},
                        data_holder = "server",
                        endpoint_id = 2,
                        id = 2821,
                        name = "diagnostics"
                    }
                },
                device_id = 263,
                id = 2,
                profile_id = 260
            }
        },
        id = 42,
        is_battery_powered = true,
        is_reachable = false,
        manufacturer_id = "CentraLite",
        model_id = model,
        reachable_state_timeout = 3600,
        security = false
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

    local tampering_cover_enum = loadfile("HUB:zigbee/scripts/resources/enums/tampering_cover_events")()

    local expected_motion_items = {
        {
        name = "motion",
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        value_type = "bool",
        value = false
    },
        {
            name = "tampering_cover_alarm",
            endpoint_id = 1,
            cluster_name = "ias_zone",
            attribute_name = "ZoneStatus",
            value_type = "token",
            enum = {
                tampering_cover_enum.unknown,
                tampering_cover_enum.no_tampering_cover,
                tampering_cover_enum.tampering_cover },
            value = tampering_cover_enum.unknown
        },
        {
            name = "security_threat",
            endpoint_id = 1,
            cluster_name = "ias_zone",
            attribute_name = "ZoneStatus",
            tag = ".security_tag",
            main_item_name = "motion",
        },
        {
            name = "battery",
            endpoint_id = 1,
            cluster_name = "power_configuration",
            attribute_name = "BatteryVoltage",
            value_type = "int",
            value = 100
        }
    }


    local expected_devices = {
        {
            name = "Motion Sensor",
            category = "security_sensor",
            subcategory = "motion",
            items = expected_motion_items,
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
            value = "3305-Geu",
            description = "test with 3305-Geu model"
        }
    }
)
