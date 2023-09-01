local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Token array setting name",
        description = "Token array setting description",
        type = "array.token",
        enum = {
            value_1 = {
                text = "Value 1",
                value = 1
            },
            value_2 = {
                text = "Value 2",
                value = 2
            },
            value_3 = {
                text = "Value 3",
                value = 4
            }
        },
        configuration = {
            number = 42,
            size = 1,
            value = 3
        }
    }
})

fixture.add_node()

fixture.update_configuration_value(42, 1, 3)
fixture.check_set_setting_value_called({ "value_1", "value_2" }, "synced")

fixture.update_configuration_value(42, 1, 6)
fixture.check_set_setting_value_called({ "value_2", "value_3" }, "synced")

fixture.update_configuration_value(42, 1, 4)
fixture.check_set_setting_value_called({ "value_3" }, "synced")

fixture.remove_node()
