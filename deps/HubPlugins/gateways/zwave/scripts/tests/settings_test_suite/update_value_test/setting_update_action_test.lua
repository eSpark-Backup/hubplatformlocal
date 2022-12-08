local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Action setting name",
        description = "Action setting description",
        type = "action",
        value = 0xFF,
        label = {
            text = "Button label",
        },
        configuration = {
            number = 42,
            size = 1
        }
    }
})

fixture.add_node()

fixture.update_configuration_value(42, 1, 0)
fixture.check_set_setting_value_not_called()

fixture.remove_node()
