local tests = require( "tests" )

local fan_mode_enum = require("HUB:zigbee/scripts/resources/enums/thermostat_fan_mode")

local function test_impl( param )
    local cluster_fan_control = { 
        name = "fan_control",
        attributes = {
            {
                name = "FanMode",
                formatted_value = "off"
            },
            {
                name = "FanModeSequence",
                formatted_value = param.sequence
            }
        }
    }
    
    local endpoint_1 = { 
        id = 1,
        clusters = { cluster_fan_control }
    }
    
    local node = {
        id = 42,
        manufacturer_id = "Ezlo",
        model_id = "Thermostat",
        security = 0,
        is_reachable = 1,
        is_battery_powered = false,
    
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
    
    -- check adding result
    local thermostat_mode_enum = require("HUB:zigbee/scripts/resources/enums/thermostat_mode")
    
    local expected_items = {
        {
            name = "thermostat_fan_mode",
            endpoint_id = 1,
            cluster_name = "fan_control",
            attribute_name = "FanMode",
            value_type = "token",
            enum = param.expected_enum,
            value = param.expected_enum[1]
        }
    }
    
    
    local expected_devices = {
        {
            name = "Unknown",
            category = "unknown",
            items = expected_items,
            node_id = 42,
            battery_powered = false
        }
    }
    
    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {}
    }
    
    local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"
    
    loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices, check_context )
    
    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = 42,     
        manufacturer_id = "Ezlo",
        model_id = "Thermostat"
    } )
    
    -- check node removing result
    loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )

end


tests.run_with_params( 
    test_impl, 
    {
        {
            value = {
                sequence = {"low", "medium", "high"},
                expected_enum = { 
                    fan_mode_enum.low, 
                    fan_mode_enum.medium, 
                    fan_mode_enum.high 
                }
            },
            description = "test with low/medium/high"
        },
        {
            value = {
                sequence = {"low", "high"},
                expected_enum = { 
                    fan_mode_enum.low, 
                    fan_mode_enum.high 
                }
            },
            description = "test with low/high"
        },
        {
            value = {
                sequence = {"low", "medium", "high", "auto"},
                expected_enum = { 
                    fan_mode_enum.low, 
                    fan_mode_enum.medium, 
                    fan_mode_enum.high,
                    fan_mode_enum.auto  
                }
            },
            description = "test with low/medium/high/auto"
        },
        {
            value = {
                sequence = {"low", "high", "auto"},
                expected_enum = { 
                    fan_mode_enum.low, 
                    fan_mode_enum.high,
                    fan_mode_enum.auto  
                }
            },
            description = "test with low/high/auto"
        },
        {
            value = {
                sequence = {"on", "auto"},
                expected_enum = { 
                    fan_mode_enum.on, 
                    fan_mode_enum.auto  
                }
            },
            description = "test with on/auto"
        }
    }
)
