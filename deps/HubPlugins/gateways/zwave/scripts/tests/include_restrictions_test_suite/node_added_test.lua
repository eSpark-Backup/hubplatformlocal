local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/include_restrictions_fixture")()
local zwave = require( "zwave" )
local core = require( "core" )
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local node = fixture.get_multisensor_node()
zwave.add_node(node)

local function test_impl(restrictions)
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)

    core.send_ui_broadcast = tests.create_mock()

    fixture:handle_event( {
        event = "node_added",
        node_id = tonumber(node.id)
    } )

    tests.assert_ne(next(core.devices), nil)
    tests.assert_called(core.send_ui_broadcast, "Check ui broadcast", {})
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
            {category = "security_sensor", subcategories = {"co", "co2"}},
            {category = "hvac"}
        }
    }
})
