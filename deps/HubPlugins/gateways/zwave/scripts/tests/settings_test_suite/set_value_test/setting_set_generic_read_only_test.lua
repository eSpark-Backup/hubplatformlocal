local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.add_node()

do -- Add setting
    fixture.set_setting_dictionary_value("abc", {
        name = "Parameter 42",
        description = "Generic zwave configuration parameter 42",
        number = 42,
        size = 2,
        hasSetter = false,
        format = "dec"
    })

    fixture.check_add_configuration_called({ 42, 2 })

    fixture.check_set_setting_dictionary_value_called("abc", {
        name = "Parameter 42",
        description = "Generic zwave configuration parameter 42",
        number = 42,
        size = 2,
        hasSetter = false,
        format = "dec",
        status = "pending"
    })

    fixture.check_request_configuration_value_called(42)
end

do -- Update setting
    fixture.set_setting_dictionary_value("abc", {
        name = "New name",
        description = "New description",
        number = 43,
        size = 4,
        hasSetter = false,
        format = "hex"
    })

    fixture.check_set_setting_dictionary_value_called("abc", {
        name = "New name",
        description = "New description",
        number = 43,
        size = 4,
        hasSetter = false,
        format = "hex",
        status = "pending"
    })

    fixture.check_remove_configuration_called(42)
    fixture.check_add_configuration_called({ 43, 4 })
    fixture.check_request_configuration_value_called(43)
end

do -- Remove setting
    fixture.set_setting_dictionary_value("abc", nil)

    fixture.check_set_setting_dictionary_value_called("abc", {})

    fixture.check_remove_configuration_called(43)
end

fixture.remove_node()
