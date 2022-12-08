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

fixture.set_setting_value()
fixture.check_update_setting_status_called("pending")
fixture.check_set_configuration_value_called(42, 1, 0xFF)

fixture.remove_node()
