local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

zwave.request_node_values = tests.create_mock()

local function test_impl(params)
    local channel_0 = { 
        id = 0,
        classes = {}
    }
    
    local node = {
        id = "9000",
        manufacturer_id = params.manufacturer_id,
        product_type = 2,
        product_id = 3,
        is_battery_powered = false,
    
        specific_type = params.specific_type,
        generic_type = "GENERIC_TYPE_STATIC_CONTROLLER",
        
        icon_type = "ICON_TYPE_GENERIC_CENTRAL_CONTROLLER",
    
        security = { "s0" },
        security_class_started = { "s0" },
    
        channels = { channel_0 }
    }
    
    local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

    local zwave = require( "zwave" )
    zwave.request_node_values = tests.create_mock()
    
    -- prepare zwave
    zwave.add_node( node )
    
    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000 
    } )
    
    
    local expected_devices = {
        {
            name = params.device_name,
            category = "generic_io",
            subcategory = "controller",
            items = {},
            node_id = "9000",
            battery_powered = false,
            security = "middle"
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
        manufacturer_id = 1,
        product_type = 2,
        product_id = 3, 
    } )
    
    -- check node removing result
    loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params( 
    test_impl, 
    {
        {
            value = {
                specific_type = "SPECIFIC_TYPE_GATEWAY",
                manufacturer_id = 1,
                device_name = "Controller"
            },
            description = "test with SPECIFIC_TYPE_GATEWAY"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_NOT_USED",
                manufacturer_id = 1,
                device_name = "Controller"
            },
            description = "test with SPECIFIC_TYPE_NOT_USED"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_PC_CONTROLLER",
                manufacturer_id = 1,
                device_name = "Controller"
            },
            description = "test with SPECIFIC_TYPE_PC_CONTROLLER"
        },
        {
            value = {
                specific_type = "SPECIFIC_TYPE_PC_CONTROLLER",
                manufacturer_id = 603,
                device_name = "Ezlo Controller"
            },
            description = "test with Ezlo identifier"
        }
    }
)
