local tests = require( "tests" )

local function test_impl(params)
    local cluster_door_lock = { 
        name = "door_lock",
        attributes = {
            {
                name = "LockState",
                formatted_value = "locked"
            },
            {
                name = "MinPINCodeLength",
                formatted_value = params.min_length
            },
            {
                name = "MaxPINCodeLength",
                formatted_value = params.max_length
            }
        },
        commands = {
            {
                name = "SetPinCode"
            },
            {
                name = "GetPinCode"
            },
            {
                name = "ClearPinCode"
            }
        }
    }
    
    local endpoint_1 = { 
        id = 1,
        clusters = { cluster_door_lock, }
    }
    
    local node = {
        id = 42,
        manufacturer_id = "Ezlo",
        model_id = "Door Lock",
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
    
    local door_lock_state = require("HUB:zigbee/scripts/resources/enums/door_lock_state")
    
    -- check adding result
    local expected_items = {
        {
            name = "door_lock",
            endpoint_id = 1,
            cluster_name = "door_lock",
            attribute_name = "LockState",
            value_type = "token",
            enum = {
                door_lock_state.unsecured,
                door_lock_state.secured,
                door_lock_state.unknown
            },
            value = door_lock_state.unknown
        },
        {
            name = "user_codes",
            endpoint_id = 1,
            cluster_name = "door_lock",
            attribute_name = "",
            value_type = "dictionary.userCode",
            elements_max_number = 100,
            user_code_restriction = params.restriction,
            value = {}
        },
    {
        name = "user_codes_scan_progress",
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = "",
        value_type = "int",
        value = 100
    }
    }
    
    local expected_devices = {
        {
            name = "Door Lock",
            category = "door_lock",
            items = expected_items,
            node_id = 42,
            battery_powered = true
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
        model_id = "Door Lock"
    } )
    
    -- check node removing result
    loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
end

tests.run_with_params( 
    test_impl, 
    {
        {
            value = {
                min_length = 4,
                max_length = 4,
                restriction = "^\\d{4}$"
            },
            description = "test with fixed length"
        },
        {
            value = {
                min_length = 2,
                max_length = 6,
                restriction = "^\\d{2,6}$"
            },
            description = "test with not fixed length"
        }
    }
)
