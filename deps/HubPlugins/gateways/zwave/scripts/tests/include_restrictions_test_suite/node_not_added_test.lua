local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/include_restrictions_fixture")()
local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zwave/scripts/helpers")
local zwave = require( "zwave" )
local core = require( "core" )
local tests = require("tests")

local node = fixture.get_multisensor_node()
zwave.add_node(node)

local function test_impl(restrictions)
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)

    core.add_device = tests.create_mock()
    core.add_item = tests.create_mock()
    core.send_ui_broadcast = tests.create_mock()

    fixture:handle_event( {
        event = "node_added",
        node_id = tonumber(node.id)
    } )

    tests.assert_called(core.add_device, "Check no device added", {})
    tests.assert_called(core.add_item, "Check no items added", {})
    tests.assert_called(core.send_ui_broadcast, "Check ui broadcast", {{{ event = "restricted_device_not_added" }}})
    fixture.check_node_restricted(9000, true)

    restrictions_helper:call("storage/remove_restricted_node_id", 9000)
    fixture.reset_restrictions()
end

tests.run_with_params(test_impl, {
    {
        value = {
            {category = "switch"},
            {category = "security_sensor"},
            {category = "hvac"}
        }
    },
    {
        value = {
            {category = "switch"},
            {category = "security_sensor", subcategories = {"co", "door"}},
            {category = "hvac"}
        }
    },
    {
        value = {
            {category = "switch"},
            {category = "temperature"},
            {category = "hvac"}
        }
    },
    {
        value = {
            {category = "switch"},
            {category = "security_sensor", subcategories = {"co", "motion"}},
            {category = "hvac"}
        }
    }
})
