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

fixture.update_configuration_value(42, 1, 0)
fixture.check_set_setting_value_called(false, "synced")

fixture.update_configuration_value(42, 1, 1)
fixture.check_set_setting_value_called(true, "synced")

fixture.update_configuration_value(42, 1, 2)
fixture.check_set_setting_value_called(true, "synced")

fixture.remove_node()
