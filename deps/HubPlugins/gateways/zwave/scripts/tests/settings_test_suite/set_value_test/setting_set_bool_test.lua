local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

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

fixture.set_setting_value(false)
fixture.check_set_setting_value_called(false, "pending")
fixture.check_set_configuration_value_called(42, 1, 0)

fixture.set_setting_value(true)
fixture.check_set_setting_value_called(true, "pending")
fixture.check_set_configuration_value_called(42, 1, 1)

fixture.remove_node()
