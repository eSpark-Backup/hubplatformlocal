local fixture = hubplugins.weak_loadfile("HUB:zigbee/scripts/tests/fixtures/include_restrictions_fixture")()

local tests = require("tests")

local function test_wrong_format(restrictions)
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(nil)
    fixture.reset_restrictions()
end

tests.run_with_params(test_wrong_format, {
    {
        value = "test_string"
    },
    {
        value = 15
    },
    {
        value = {test = "not_array"}
    },
    {
        value = {{subcategories = {"test_sub_1", "test_sub_2"}}}
    },
    {
        value = {{category = 1}}
    },
    {
        value = {{category = "switch", subcategories = "test"}}
    },
    {
        value = {{category = "switch", subcategories = {test = "test"}}}
    },
    {
        value = {{category = "switch", subcategories = {1, 2}}}
    },
})



local function test_correct_format(restrictions)
    fixture.setup_restrictions(restrictions)
    fixture.check_restrictions(restrictions)
    fixture.reset_restrictions()
end

tests.run_with_params(test_correct_format, {
    {
        value = {}
    },
    {
        value = {{category = "switch"}}
    },
    {
        value = {{category = "switch"}, {category = "security_sensor"}}
    },
    {
        value = {{category = "switch", subcategories = {"test1"}}}
    },
    {
        value = {{category = "switch", subcategories = {"test1", "test2"}}}
    },
    {
        value = {
            {category = "switch", subcategories = {"test1", "test2", "test3"}},
            {category = "security_sensor", subcategories = {"test4", "test5"}},
            {category = "thermostat"}
        }
    },
})
