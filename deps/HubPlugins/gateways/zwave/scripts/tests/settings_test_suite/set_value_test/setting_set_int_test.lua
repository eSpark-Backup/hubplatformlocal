local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Integer setting name",
        description = "Integer setting description",
        type = "int",
        value_min = 0,
        value_max = 255,
        configuration = {
            number = 42,
            size = 2,
            value = 71
        }
    }
})

fixture.add_node()

fixture.set_setting_value(0)
fixture.check_set_setting_value_called(0, "pending")
fixture.check_set_configuration_value_called(42, 2, 0)

fixture.set_setting_value(123)
fixture.check_set_setting_value_called(123, "pending")
fixture.check_set_configuration_value_called(42, 2, 123)

fixture.set_setting_value(255)
fixture.check_set_setting_value_called(255, "pending")
fixture.check_set_configuration_value_called(42, 2, 255)

fixture.remove_node()
