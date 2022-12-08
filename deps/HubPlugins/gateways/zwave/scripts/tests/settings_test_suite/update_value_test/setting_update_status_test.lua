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
            number = 42,
            size = 1,
            value = 0
        }
    }
})

fixture.add_node()

do -- Add setting
    loadfile("HUB:zwave/scripts/set_setting_dictionary_value")({
        setting_id = fixture.get_generic_setting_id(),
        key = "abc",
        value = {
            name = "Parameter 42",
            description = "Generic zwave configuration parameter 42",
            number = 42,
            size = 1,
            value = 0,
            format = "dec"
        }
    })

    tests.assert_called(core.set_setting_dictionary_value, "check calls to core.set_setting_dictionary_value")
    tests.assert_called(zwave.set_configuration_value, "check calls to zwave.set_configuration_value")
end

do -- Update statuses of both generic and non-generic settings
    fixture.update_configuration_status(42, "failed")

    fixture.check_update_setting_status_called("failed")
    fixture.check_update_setting_dictionary_status_called("abc", "failed")
end

fixture.remove_node()
