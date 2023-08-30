local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local function test_impl(params)
    local cc_basic = { 
        id = class_ids.basic,
    }
    
    local channel_0 = { 
        id = 0,
        classes = { cc_basic }
    }
    
    local node = {
        id = "9000",
        manufacturer_id = 1,
        product_type = 2,
        product_id = 3,
        is_battery_powered = true,
    
        specific_type = params.specific_type,
        generic_type = params.generic_type,
        icon_type = params.icon_type,
    
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
    local expected_switch_items = {
        {
            name = "switch",
            cc_id = class_ids.basic, 
            channel_id = 0,
        }
    }
    
    local expected_devices = {
        {
            name = params.device_name,
            category = params.device_category,
            subcategory = params.device_subcategory,
            items = expected_switch_items,
            node_id = "9000",
            battery_powered = true
        }
    }
    
    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {}
    }
    
    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
    
    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )
    
    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",     
        manufacturer_id = 1,
        product_type = 2,
        product_id = 3
    } )
    
    -- check node removing result
    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                specific_type = "SPECIFIC_TYPE_NOT_USED",
                generic_type = "GENERIC_TYPE_SWITCH_BINARY",
                
                device_name = "In Wall Switch",
                device_category = "switch",
                device_subcategory = "in_wall"
            },
            description = "test with GENERIC_TYPE_SWITCH_BINARY"
        },
        
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH",
                
                device_name = "Switch Interrior",
                device_category = "switch",
                device_subcategory = "interior_plugin"
            },
            description = "test with ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_PLUGIN",
                
                device_name = "Switch Interrior",
                device_category = "switch",
                device_subcategory = "interior_plugin"
            },
            description = "test with ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_PLUGIN"
        },
        
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_WALL_OUTLET",
                
                device_name = "In Wall Switch",
                device_category = "switch",
                device_subcategory = "in_wall"
            },
            description = "test with ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_WALL_OUTLET"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_CEILING_OUTLET",
                
                device_name = "In Wall Switch",
                device_category = "switch",
                device_subcategory = "in_wall"
            },
            description = "test with ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_CEILING_OUTLET"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_GENERIC_ON_OFF_WALL_SWITCH",
                
                device_name = "In Wall Switch",
                device_category = "switch",
                device_subcategory = "in_wall"
            },
            description = "test with ICON_TYPE_GENERIC_ON_OFF_WALL_SWITCH"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_ONE_BUTTON",
                
                device_name = "In Wall Switch",
                device_category = "switch",
                device_subcategory = "in_wall"
            },
            description = "test with ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_ONE_BUTTON"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_TWO_BUTTONS",
                
                device_name = "In Wall Switch",
                device_category = "switch",
                device_subcategory = "in_wall"
            },
            description = "test with ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_TWO_BUTTONS"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_THREE_BUTTONS",
                
                device_name = "In Wall Switch",
                device_category = "switch",
                device_subcategory = "in_wall"
            },
            description = "test with ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_THREE_BUTTONS"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_FOUR_BUTTONS",
                
                device_name = "In Wall Switch",
                device_category = "switch",
                device_subcategory = "in_wall"
            },
            description = "test with ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_FOUR_BUTTONS"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_UNKNOWN",
                generic_type = "GENERIC_TYPE_UNKNOWN",
                icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_ONE_ROTARY",
                
                device_name = "In Wall Switch",
                device_category = "switch",
                device_subcategory = "in_wall"
            },
            description = "test with ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_ONE_ROTARY"
        },
    }
)
