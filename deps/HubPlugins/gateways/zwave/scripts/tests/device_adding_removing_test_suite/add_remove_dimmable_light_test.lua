local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local tests = require( "tests" )
local zwave = require( "zwave" )

local fixture = hubplugins.weak_loadfile( "HUB:zwave/scripts/tests/fixtures/dimmable_light_fixture" )()

zwave.request_node_values = tests.create_mock()


local function test_impl( product_info )
    -- prepare zwave
    zwave.add_node( fixture:create_node( "9000", product_info ) )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )

    local expected_devices = {
        {
            name = "In Wall Dimmable",
            category = "dimmable_light",
            subcategory = "dimmable_in_wall",
            node_id = "9000",
            battery_powered = true,
            items = {
                {
                    name = item_names.switch,
                    cc_id = class_ids.dimmer,
                    channel_id = 0,
                },
                {
                    name = item_names.dimmer,
                    cc_id = class_ids.dimmer,
                    channel_id = 0,
                },
                {
                    name = item_names.dimmer_up,
                    cc_id = class_ids.dimmer,
                    channel_id = 0,
                },
                {
                    name = item_names.dimmer_down,
                    cc_id = class_ids.dimmer,
                    channel_id = 0,
                },
                {
                    name = item_names.dimmer_stop,
                    cc_id = class_ids.dimmer,
                    channel_id = 0,
                },
            },
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
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id
    } )

    -- check node removing result
    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params( test_impl, fixture:get_autodetection_specifics() )
