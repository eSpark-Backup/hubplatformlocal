local fixture = hubplugins.weak_loadfile("HUB:zigbee/scripts/tests/fixtures/include_restrictions_fixture")()
local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zigbee/scripts/helpers")
local tests = require("tests")

restrictions_helper:call("storage/insert_restricted_node_id", 9000)
fixture.check_node_restricted(9000, true)

fixture:handle_event({
    event = "node_removed",
    node_id = 9000,
    manufacturer_id = "Test",
    model_id = "Test"
})

fixture.check_node_restricted(9000, false)
