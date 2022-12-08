local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()
zwave.set_value = tests.create_mock()
zwave.request_value = tests.create_mock()

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/window_cov_shutters_fixture")()

local function test_impl( product_info )
    -- prepare zwave
    zwave.add_node(fixture:create_node( "9000", product_info ) )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000 
    } )

    -- send goto_favorite event
    local item_id = storage:call("find_item_id", "goto_favorite", 9000, class_ids.dimmer, 0, "dimmer" )

    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = item_id, value = true })
    tests.assert_called(zwave.set_value, "check dimmer set true 1", { { "9000", class_ids.dimmer, 0, 0xFF } } )

    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = item_id, value = true })
    tests.assert_called(zwave.set_value, "check dimmer set true 2", { { "9000", class_ids.dimmer, 0, 0xFF } } ) --again

    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = item_id, value = false })
    tests.assert_called(zwave.set_value, "check dimmer set false (no call)", { } )

    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = item_id, value = true })
    tests.assert_called(zwave.set_value, "check dimmer set true 3", { { "9000", class_ids.dimmer, 0, 0xFF } } ) --again

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",     
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id
    } )
end

tests.run_with_params( test_impl, fixture:get_manufacturer_specifics() )
tests.run_with_params( test_impl, fixture:get_autodetection_specifics() )
