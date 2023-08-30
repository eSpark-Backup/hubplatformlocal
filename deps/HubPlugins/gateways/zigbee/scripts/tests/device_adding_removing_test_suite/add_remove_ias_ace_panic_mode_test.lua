local tests = require( "tests" )
local house_modes = require( "house_modes" )
local emergency_state = loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()

house_modes.get_current = function() return "1" end

local function test_impl(params)

    local cluster_ias_ace = { 
        name = "ias_ace",
        data_holder = "client",
        attributes = {},
        commands = {
            {
                name = "Arm"
            }
        }
    }
    
    for _, command in ipairs(params.commands) do
        table.insert(cluster_ias_ace.commands, command)
    end
    
    local endpoint_1 = { 
        id = 1,
        clusters = { cluster_ias_ace }
    }
    
    local node = {
        id = 42,
        manufacturer_id = "Ezlo",
        model_id = "Keypad",
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
    
    local expected_panic_button_items = {
        {
            name = "emergency_state",
            endpoint_id = 1,
            cluster_name = "ias_ace",
            attribute_name = "",
            value_type = "token",
            enum = params.enum,
            value = emergency_state.idle
        },
        {
            name = "security_threat",
            endpoint_id = 1,
            cluster_name = "ias_ace",
            attribute_name = "",
            tag = ".security_tag",
            main_item_name = "emergency_state",
        }
    }
    
    
    local expected_devices = {
        {
            name = "Keypad",
            category = "keypad",
            items = expected_security_items,
            node_id = 42,
            battery_powered = true,
            
            subdevices = {
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
        device_ids = {}
    }
    
    local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"
    
    loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices, check_context )
    
    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = 42,     
        manufacturer_id = "Ezlo",
        model_id = "Keypad"
    } )
    
    -- check node removing result
    loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
end

tests.run_with_params( 
    test_impl, 
    {
        {
            value = {
                commands = {{ name = "Panic" }},
                enum = {
                    emergency_state.idle,
                    emergency_state.panic },
            },
            description = "test with panic"
        },
        {
            value = {
                commands = {{ name = "Emergency" }},
                enum = {
                    emergency_state.idle,
                    emergency_state.emergency },
            },
            description = "test with emergency"
        },
        {
            value = {
                commands = {{ name = "Panic" }, { name = "Emergency" }},
                enum = {
                    emergency_state.idle,
                    emergency_state.emergency,
                    emergency_state.panic },
            },
            description = "test with panic and emergency"
        }
    }
)
