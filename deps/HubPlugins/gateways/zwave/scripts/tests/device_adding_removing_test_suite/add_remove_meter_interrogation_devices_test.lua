local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local timer = require( "timer" )

zwave.request_node_values = tests.create_mock()

timer.set_interval_with_id = tests.create_mock()
timer.cancel = tests.create_mock()

local function test_impl( product_info )
    local cc_meter = {
        id = class_ids.meter,
        reset = 0,
        type = 1,
        scales = {},
    }

    local channel_0 = {
        id = 0,
        classes = { cc_meter, }
    }

    local node = {
        id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",

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

    local timer_id
    tests.assert_called(
        timer.set_interval_with_id,
        "check set up interval timer",
        function( _, _, id, script, script_params )
            timer_id = id

            tests.assert_eq( script, "HUB:zwave/scripts/timer_handlers/common/request_value", "check timer script" )
            tests.assert_eq( script_params, { node_id = 9000, channel_id = 0, class_id = class_ids.meter },"check set up timer" )
         end,
        1
    )

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = 9000,
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id
    } )

    tests.assert_called( timer.cancel, "check cancel timer", { { timer_id } } )
end

tests.run_with_params( test_impl,
{
    {
        value = {
            manufacturer_id = 271,
            product_type = 1536,
            product_id = 4096,
        },
        description = "Fibaro Wall Plug FGWPE/F-102 271_1536_4096"
    },
    {
        value = {
            manufacturer_id = 45,
            product_type = 2,
            product_id = 84,
        },
        description = "Qubino Smart Plug 16A 45_2_84"
    },
} )