local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Temperature setting name",
        description = "Temperature setting description",
        type = "temperature",
        scale = "celsius",
        value_min = -100,
        value_max = 100,
        configuration = {
            number = 42,
            size = 1,
            value = 71
        }
    }
})

fixture.add_node()

fixture.set_setting_value({ value = 0, scale = "celsius" })
fixture.check_set_setting_value_called({ value = 0, scale = "celsius" }, "pending")
fixture.check_set_configuration_value_called(42, 1, 0)

fixture.set_setting_value({ value = 123, scale = "fahrenheit" })
fixture.check_set_setting_value_called({ value = 123, scale = "fahrenheit" }, "pending")
--fixture.check_set_configuration_value_called(42, 1, 51) -- Fixme
fixture.check_set_configuration_value_called(42, 1, 123)

fixture.set_setting_value({ value = 255, scale = "kelvin" })
fixture.check_set_setting_value_called({ value = 255, scale = "kelvin" }, "pending")
-- fixture.check_set_configuration_value_called(42, 1, -18) -- Fixme
fixture.check_set_configuration_value_called(42, 1, 255)

fixture.remove_node()
