local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local function test_impl( product_info )
    local cc_switch = {
        id = class_ids.switch_binary,
    }

    local cc_basic = {
        id = class_ids.basic,
    }

    local channel_0 = {
        id = 0,
        classes = { cc_switch, cc_basic }
    }

    local node = {
        id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "GENERIC_TYPE_SWITCH_BINARY",

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
            cc_id = class_ids.switch_binary,
            channel_id = 0,
        },
        {
            name = "switch",
            cc_id = class_ids.basic,
            channel_id = 0,
            is_merged = true
        }
    }

    local expected_devices = {
        {
            name = "Water Valve",
            category = "switch",
            subcategory = "valve",
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

    -- send switch basic event
    local switch_item = function() return core.get_item( storage:call("find_item_id", "switch", 9000, class_ids.switch_binary, 0  ) ) end
    local basic_item = function() return core.get_item( storage:call("find_item_id", "basic", 9000, class_ids.basic, 0  ) ) end

    events_handler( {
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.basic,
        value = 0xFF
    } )

    tests.assert_eq( switch_item().value, true, "check switch value after basic value changing" )

    -- send switch event
    events_handler( {
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.switch_binary,
        value = 0
    } )

    tests.assert_eq( switch_item().value, false, "check motion value after switch value changing" )

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
            manufacturer_id = 132,
            product_type = 529,
            product_id = 43527,
        },
        description = "132_529_43527"
    },
    {
        value = {
            manufacturer_id = 132,
            product_type = 529,
            product_id = 43526,
        },
        description = "132_529_43526"
    }
} )
