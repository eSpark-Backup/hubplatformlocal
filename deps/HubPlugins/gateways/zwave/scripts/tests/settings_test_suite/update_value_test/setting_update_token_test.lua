local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Token setting name",
        description = "Token setting description",
        type = "token",
        enum = {
            value_1 = {
                text = "Value 1",
                value = 0
            },
            value_2 = {
                text = "Value 2",
                value = 1
            },
            value_3 = {
                text = "Value 3",
                value = 2
            }
        },
        configuration = {
            number = 42,
            size = 1,
            value = 1
        }
    }
})

fixture.add_node()

fixture.update_configuration_value(42, 1, 0)
fixture.check_set_setting_value_called("value_1", "synced")

fixture.update_configuration_value(42, 1, 1)
fixture.check_set_setting_value_called("value_2", "synced")

fixture.update_configuration_value(42, 1, 2)
fixture.check_set_setting_value_called("value_3", "synced")

fixture.remove_node()
