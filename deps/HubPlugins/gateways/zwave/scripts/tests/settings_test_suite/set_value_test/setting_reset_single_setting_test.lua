local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")
local tests = require("tests")
local core = require("core")
local zwave = require("zwave")

fixture.set_device_settings({
    {
        name = "Boolean setting name",
        description = "Boolean setting description",
        type = "bool",
        configuration = {
            number = 41,
            size = 1
        }
    },
    {
        name = "Boolean setting name",
        description = "Boolean setting description",
        type = "bool",
        configuration = {
            number = 42,
            size = 1
        }
    }
})

fixture.add_node()

do -- Add setting
    loadfile("HUB:zwave/scripts/set_setting_dictionary_value")({
        setting_id = fixture.get_generic_setting_id(),
        key = "aaa",
        value = {
            name = "Parameter 2",
            description = "Generic zwave configuration parameter 2",
            number = 3,
            size = 1,
            value = 0,
            format = "dec"
        }
    })

    tests.assert_called(core.set_setting_dictionary_value, "check calls to core.set_setting_dictionary_value")
    tests.assert_called(zwave.set_configuration_value, "check calls to zwave.set_configuration_value")
end

fixture.reset_setting(fixture.get_setting_id(2))
fixture.check_reset_configuration_value_called(42)

fixture.reset_setting(fixture.get_generic_setting_id())
fixture.check_reset_configuration_value_called(3)

fixture.remove_node()
