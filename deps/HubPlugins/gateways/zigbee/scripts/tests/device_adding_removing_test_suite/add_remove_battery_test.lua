local tests = require( "tests" )

local fan_mode_enum = require("HUB:zigbee/scripts/resources/enums/thermostat_fan_mode")

local function test_impl( param )
    local cluster_power_configuration = { 
        name = "power_configuration",
        attributes = param.attributes
    }
    
    local endpoint_1 = { 
        id = 1,
        clusters = { cluster_power_configuration }
    }
    
    local endpoint_2 = { 
        id = 2,
        clusters = { cluster_power_configuration }
    }
    
    local node = {
        id = 42,
        manufacturer_id = "Ezlo",
        model_id = "Battery Device",
        security = 0,
        is_reachable = 1,
        is_battery_powered = true,
    
        endpoints = { endpoint_1, endpoint_2 }
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
    
    local expected_items = {
        {
            name = "battery",
            endpoint_id = 1,
            cluster_name = "power_configuration",
            attribute_name = param.item_attribute,
            value_type = "int",
            value = 100
        }
    }
    
    
    local expected_devices = {
        {
            name = "Unknown",
            category = "unknown",
            items = expected_items,
            node_id = 42,
            battery_powered = true
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
                attributes = {
                    {
                        name = "BatteryPercentageRemaining",
                        formatted_value = 200
                    }
                },
                item_attribute = "BatteryPercentageRemaining"
            },
            description = "test with BatteryPercentageRemaining"
        },
        {
            value = {
                attributes = {
                    {
                        name = "BatteryVoltage",
                        formatted_value = 30
                    }
                },
                item_attribute = "BatteryVoltage"
            },
            description = "test with BatteryVoltage"
        },
        {
            value = {
                attributes = {
                    {
                        name = "BatteryVoltage",
                        formatted_value = 30
                    },
                    {
                        name = "BatteryPercentageRemaining",
                        formatted_value = 200
                    }
                },
                item_attribute = "BatteryPercentageRemaining"
            },
            description = "test with BatteryPercentageRemaining and BatteryVoltage"
        }
    }
)
