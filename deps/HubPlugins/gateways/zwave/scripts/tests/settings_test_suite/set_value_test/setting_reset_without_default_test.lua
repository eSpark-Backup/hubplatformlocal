local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Boolean setting name",
        description = "Boolean setting description",
        type = "bool",
        configuration = {
            number = 42,
            size = 1
        }
    }
})

fixture.add_node()

fixture.reset_settings()
fixture.check_reset_configuration_value_called(42)

fixture.remove_node()
