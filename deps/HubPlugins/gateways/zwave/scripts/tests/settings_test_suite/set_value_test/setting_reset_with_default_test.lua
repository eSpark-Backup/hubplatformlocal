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

fixture.reset_settings()
fixture.check_set_configuration_value_called(42, 1, 0)

fixture.remove_node()
