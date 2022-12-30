local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/include_restrictions_fixture")()

local tests = require("tests")
local zwave = require("zwave")

local restrictions = {
    {category = "switch", subcategories = {"test1", "test2", "test3"}},
    {category = "security_sensor", subcategories = {"test4", "test5"}},
    {category = "thermostat"}
}

do
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)

    fixture:handle_event({
        event = "include_finished_error",
        error = "test"
    })

    fixture.check_restrictions(nil)
end

do
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)

    fixture:handle_event({
        event = "include_finished_timeout",
        error = "test"
    })

    fixture.check_restrictions(nil)
end

do
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)

    fixture:handle_event({
        event = "include_finished",
    })

    fixture.check_restrictions(nil)
end
