local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")
local tests = require("tests")
local core = require("core")
local zwave = require("zwave")

fixture.add_node()

do -- Add setting
    loadfile("HUB:zwave/scripts/set_setting_dictionary_value")({
        setting_id = fixture.get_generic_setting_id(),
        key = "aaa",
        value = {
            name = "Parameter 42",
            description = "Generic zwave configuration parameter 42",
            number = 42,
            size = 1,
            has_setter = false,
            format = "dec"
        }
    })

    tests.assert_called(core.set_setting_dictionary_value, "check calls to core.set_setting_dictionary_value")
    fixture.check_request_configuration_value_called(42)
end

fixture.reset_settings()
fixture.check_reset_configuration_value_not_called()

fixture.remove_node()
