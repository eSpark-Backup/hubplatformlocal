local fixture = hubplugins.weak_loadfile("HUB:zigbee/scripts/tests/fixtures/include_restrictions_fixture")()

local restrictions = {
    {category = "switch", subcategories = {"test1", "test2", "test3"}},
    {category = "security_sensor", subcategories = {"test4", "test5"}},
    {category = "thermostat"}
}

do
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)

    fixture:handle_event({
        event = "include_device",
        status = "finished"
    })

    fixture.check_restrictions(nil)
end

do
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)

    fixture:handle_event({
        event = "include_device",
        status = "failed"
    })

    fixture.check_restrictions(nil)
end

do
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)

    fixture:handle_event({
        event = "include_device",
        status = "timeout"
    })

    fixture.check_restrictions(nil)
end
