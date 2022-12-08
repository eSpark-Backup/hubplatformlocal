local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local timer = require( "timer" )

timer.set_timeout_with_id = tests.create_mock()
timer.cancel = tests.create_mock()

local function test_impl( product_info )
    local cc_dimmer = {
        id = class_ids.dimmer,
    }

    local cc_switch = {
        id = class_ids.switch_binary,
    }

    local cc_basic = {
        id = class_ids.basic,
    }

    local channel_0 = {
        id = 0,
        classes = { cc_dimmer, cc_basic, cc_switch }
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

    -- check adding result
    local expected_dimmer_items = {
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "basic",
            cc_id = class_ids.basic,
            channel_id = 0,
        },
        {
            name = "switch",
            cc_id = class_ids.switch_binary,
            channel_id = 0
        },
    }

    local expected_devices = {
        {
            name = "Light",
            category = "dimmable_light",
            subcategory = "dimmable_bulb",
            items = expected_dimmer_items,
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

    -- send dimmer basic event
    local dimmer_item = function() return core.get_item( storage:call("find_item_id", "dimmer", 9000, class_ids.dimmer, 0  ) ) end
    local basic_item = function() return core.get_item( storage:call("find_item_id", "basic", 9000, class_ids.basic, 0  ) ) end

    local request_value_timer_id = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", 9000, 0, class_ids.dimmer)._timer_id
    local timer_params = { 2000, request_value_timer_id, "HUB:zwave/scripts/timer_handlers/common/request_value", tests.skipped_param }

    events_handler( {
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.basic,
        value = 0x11
    } )

    tests.assert_eq( dimmer_item().value, 0x11, "check switch value after basic value changing" )
    tests.assert_eq( basic_item().value, false, "check basic value after basic value changing" )
    tests.assert_called( timer.set_timeout_with_id,
                         "check timer.set_timeout_with_id called once after basic value changing",
                         { timer_params } )

    -- send dimmer event
    events_handler( {
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.dimmer,
        value = 0x05,
        target_value = 0x11
    } )

    tests.assert_eq( dimmer_item().value, 0x05, "check motion value after dimmer value changing" )
    tests.assert_eq( basic_item().value, false, "check basic value after dimmer value changing" )
    tests.assert_called( timer.set_timeout_with_id,
                         "check timer.set_timeout_with_id called once after dimmer value changing",
                         { timer_params } )

    -- send dimmer event
    events_handler( {
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.dimmer,
        value = 0x11,
        target_value = 0x11
    } )

    tests.assert_eq( dimmer_item().value, 0x11, "check motion value after dimmer value changing" )
    tests.assert_eq( basic_item().value, false, "check basic value after dimmer value changing" )
    tests.assert_called( timer.set_timeout_with_id, "check timer.set_timeout called once after dimmer value changing", {} )

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id
    } )

    -- check node removing result
    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end


tests.run_with_params( test_impl,
{
    {
        value = {
            manufacturer_id = 1,
            product_type = 2,
            product_id = 3,
        },
        description = "1_2_3"
    },
} )
