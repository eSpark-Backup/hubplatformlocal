local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.add_node()

fixture.set_setting_dictionary_value("abc", {
    name = "Parameter 2",
    description = "Generic zwave configuration parameter 2",
    number = 2,
    size = 2,
    value = 71,
    format = "dec"
})
fixture.check_set_setting_dictionary_value_called("abc", {
    name = "Parameter 2",
    description = "Generic zwave configuration parameter 2",
    number = 2,
    size = 2,
    value = 71,
    format = "dec",
    status = "pending"
})

fixture.update_configuration_value(2, 1, 123)
fixture.check_set_setting_dictionary_value_called("abc", {
    name = "Parameter 2",
    description = "Generic zwave configuration parameter 2",
    number = 2,
    size = 2,
    value = 123,
    format = "dec",
    status = "synced"
})

fixture.remove_node()
