local tests = require( "tests" )
local house_modes = require( "house_modes" )

house_modes.get_current = function() return "1" end

local function test_impl(model_name)
    local cluster_ias_zone = { 
        name = "ias_zone",
        attributes = {
            {
                name = "ZoneType",
                formatted_value = {zone_type = "keypad"}
            }
        }
    }
    
    local cluster_ias_ace = { 
        name = "ias_ace",
        data_holder = "client",
        attributes = {},
        commands = {
            {
                name = "Arm"
            },
            {
                name = "Panic"
            },
            {
                name = "Emergency"
            }
        }
    }
    
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
    
    local endpoint_1 = { 
        id = 1,
        clusters = { cluster_ias_zone, cluster_ias_ace, cluster_meter }
    }
    
    local node = {
        id = 42,
        manufacturer_id = "CentraLite",
        model_id = model_name,
        security = 0,
        is_reachable = 1,
        is_battery_powered = true,
    
        endpoints = { endpoint_1 }
    }
    
    local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )
    
    local zigbee = require( "zigbee" )
    
    -- prepare zigbee
    zigbee.add_node( node )
    
    -- add node
    events_handler( {
        event = "node_added",
        node_id = 42 
    } )
    
    local user_code_actions = require("HUB:zigbee/scripts/resources/enums/user_code_actions")
    local tampering_cover_enum = loadfile("HUB:zigbee/scripts/resources/enums/tampering_cover_events")()
    local emergency_state = loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()
    local zone_status_enum = loadfile("HUB:zigbee/scripts/resources/enums/zone_status")()
    
    -- check adding result
    local expected_security_items = {
        {
            name = "user_codes",
            endpoint_id = 1,
            cluster_name = "ias_ace",
            attribute_name = "",
            value_type = "dictionary.userCode",
            value = {}
        },
        {
            name = "user_code_action",
            endpoint_id = 1,
            cluster_name = "ias_ace",
            attribute_name = "",
            value_type = "user_code_action",
            value = {
                userId = -1,
                action = "unknown"
            }
        },
        {
            name = "zones_status",
            endpoint_id = 1,
            cluster_name = "ias_ace",
            attribute_name = "",
            value_type = "token",
            enum = { 
                zone_status_enum.disarmed,
                zone_status_enum.stay,
                zone_status_enum.night,
                zone_status_enum.away },
            value = zone_status_enum.disarmed
        }
    }
    
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
    
    local expected_panic_button_items = {
        {
            name = "emergency_state",
            endpoint_id = 1,
            cluster_name = "ias_zone",
            attribute_name = "ZoneStatus",
            value_type = "token",
            enum = {
                emergency_state.idle,
                emergency_state.panic },
            value = emergency_state.idle
        },
        {
            name = "emergency_state",
            endpoint_id = 1,
            cluster_name = "ias_ace",
            attribute_name = "",
            value_type = "token",
            enum = {
                emergency_state.idle,
                emergency_state.panic },
            value = emergency_state.idle,
            is_merged = true
        },
        {
            name = "security_threat",
            endpoint_id = 1,
            cluster_name = "ias_zone",
            attribute_name = "ZoneStatus",
            tag = ".security_tag",
            main_item_name = "emergency_state",
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
    
    local expected_devices = {
        {
            name = "Centralite Keypad",
            category = "keypad",
            items = expected_security_items,
            node_id = 42,
            battery_powered = true,
            
            subdevices = {
                {
                    name = "Temperature Sensor",
                    category = "temperature",
                    subcategory = "",
                    items = expected_temp_items,
                    settings = expected_temperature_settings
                },
                {
                    name = "Panic Button",
                    category = "scene_controller",
                    subcategory = "panic",
                    items = expected_panic_button_items,
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
        model_id = model_name
    } )
    
    -- check node removing result
    loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
end

tests.run_with_params( 
    test_impl, 
    {
        {
            value = "3400",
            description = "test with 3400"
        },
        {
            value = "3400-G",
            description = "test with 3400-G"
        }
    }
)
