local fixture = hubplugins.weak_loadfile("HUB:zigbee/scripts/tests/fixtures/include_restrictions_fixture")()
local zigbee = require( "zigbee" )
local core = require( "core" )
local tests = require("tests")


local node = fixture.get_multisensor_node()
zigbee.add_node(node)

local function test_impl(restrictions)
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)

    core.send_ui_broadcast = tests.create_mock()

    fixture:handle_event( {
        event = "node_added",
        node_id = tonumber(node.id)
    } )

    tests.assert_ne(next(core.devices), nil)
    tests.assert_called(core.send_ui_broadcast, "Check ui broadcast", {
        {{event = "include_device_progress", value = 100}},
        {{event = "include_device", status = "finished"}}
    })
    fixture.check_node_restricted(9000, false)

    fixture:remove_node(node)
    fixture.reset_restrictions()
end

tests.run_with_params(test_impl, {
    {
        value = {}
    },
    {
        value = {
            {category = "switch"},
            {category = "hvac"}
        }
    },
    {
        value = {
            {category = "switch"},
            {category = "security_sensor", subcategories = {"door", "co2"}},
            {category = "hvac"}
        }
    }
})
