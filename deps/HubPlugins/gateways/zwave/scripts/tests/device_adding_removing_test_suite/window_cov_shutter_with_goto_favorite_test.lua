local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/window_cov_shutters_fixture")()

local function test_impl( product_info )
    -- prepare zwave
    zwave.add_node(fixture:create_node( "9000", product_info ) )

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
            name = "goto_favorite",
            cc_id = class_ids.dimmer,
            tag = "dimmer",
            channel_id = 0  
        }
    }

    local expected_devices = {
        {
            name = product_info.name,
            category = "window_cov",
            subcategory = "window_cov",
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

    local goto_favorite_item = core.get_item( storage:call("find_item_id", "goto_favorite", 9000, class_ids.dimmer, 0, "dimmer" ) )

    tests.assert_eq(goto_favorite_item.has_getter, false, "getter of goto_favorite must be false")
    tests.assert_eq(goto_favorite_item.has_setter, true, "setter of goto_favorite must be true")

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

tests.run_with_params( test_impl, fixture:get_manufacturer_specifics() )
tests.run_with_params( test_impl, fixture:get_autodetection_specifics() )
