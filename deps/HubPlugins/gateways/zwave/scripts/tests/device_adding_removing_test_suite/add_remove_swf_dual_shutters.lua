local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/swf_shutter_fixture")()

local function test_impl( product_info )
    fixture:generate_device(9000, product_info)

    -- check adding result
    local expected_dimmer_items = {
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
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
        },
        {
            name = "shutter_command",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "shutter_state",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "shutter_second_part",
            cc_id = class_ids.dimmer,
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

    -- remove node
    fixture:remove_node(9000, product_info)

    -- check node removing result
    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params( test_impl, fixture:get_dual_shutters_manufacturer_specifics() )
