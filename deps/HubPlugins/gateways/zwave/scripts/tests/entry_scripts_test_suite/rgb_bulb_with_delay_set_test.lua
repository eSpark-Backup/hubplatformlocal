local tests = require( "tests" )

local zwave = require( "zwave" )
local timer = require "timer"
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local set_item_value = loadfile("HUB:zwave/scripts/set_item_value")

zwave.request_node_values = tests.create_mock()

local function test_impl(params)
    local channel_0 = {
        id = 0,
        classes = { 
            { id = class_ids.switch_binary },
            { id = class_ids.switch_color },
            { id = class_ids.dimmer }
        }
    }
    
    local node = {
        id = "9000",
        manufacturer_id = params.manufacturer_id,
        product_type = params.product_type,
        product_id = params.product_id,
        is_battery_powered = false,
    
        specific_type = "",
        generic_type = "",
    
        security = { "none" },
        security_class_started = { "none" },
    
        channels = { channel_0 }
    }
    
    zwave.add_node( node )
    
    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )
    
    
    local rgb_switch_id = assert(storage:call("find_item_id", item_names.rgb_color, 9000, class_ids.switch_color, 0))
    
    zwave.set_rgb_value = tests.create_mock()
    timer.set_timeout_with_id = tests.create_mock()
    
    set_item_value( {           
        item_id = rgb_switch_id,
        value = {red = 50, green = 255, blue = 128}
    } )
    
    tests.assert_called(
        zwave.set_rgb_value, 
        "check rgb value seted", 
        { { "9000", class_ids.switch_color, 0, {red = 50, green = 255, blue = 128} } }
    )

    local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", 9000, 0, class_ids.switch_color)
    
    tests.assert_called(
        timer.set_timeout_with_id, 
        "check timer created", 
        { 
            { 
                3000, 
                request_value_timer._timer_id,
                "HUB:zwave/scripts/timer_handlers/common/request_value", 
                { node_id = "9000", class_id = class_ids.switch_color, channel_id = 0 } 
            }
        }
    )
    
    
    events_handler( {
        event = "node_removed",
        node_id = "9000",     
        manufacturer_id = params.manufacturer_id,
        product_type = params.product_type,
        product_id = params.product_id
    } )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                manufacturer_id = 104,
                product_type = 3,
                product_id = 11
            },
            description = "test with Good Way Technology TD14010Z1 LED Color RGBW"
        },
        {
            value = {
                manufacturer_id = 634,
                product_type = 2306,
                product_id = 8192
            },
            description = "test with Zooz ZEN31"
        }
    }
)
