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

-- Todo: check effects of chaning scales

fixture.update_configuration_value(42, 1, 0)
fixture.check_set_setting_value_called({ value = 0, scale = "celsius" }, "synced")

fixture.update_configuration_value(42, 1, 123)
fixture.check_set_setting_value_called({ value = 123, scale = "celsius" }, "synced")

fixture.update_configuration_value(42, 1, 255)
fixture.check_set_setting_value_called({ value = 255, scale = "celsius" }, "synced")

fixture.remove_node()
