local fixture = hubplugins.weak_loadfile("HUB:zigbee/scripts/tests/fixtures/include_restrictions_fixture")()

local tests = require("tests")
local zigbee = require("zigbee")

local restrictions = {
    {category = "switch", subcategories = {"test1", "test2", "test3"}},
    {category = "security_sensor", subcategories = {"test4", "test5"}},
    {category = "thermostat"}
}

zigbee.start_include = tests.create_mock()
hubplugins.weak_loadfile("HUB:zigbee/scripts/start_include")({restrictedDeviceCategories = restrictions})

fixture.check_restrictions(restrictions)
