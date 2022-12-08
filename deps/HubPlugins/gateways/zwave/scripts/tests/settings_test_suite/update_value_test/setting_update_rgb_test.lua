local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "RGB setting name",
        description = "RGB setting description",
        type = "rgb",
        components = {
            red = 0,
            green = 1,
            blue = 2
        },
        configuration = {
            number = 42,
            size = 4,
            value = 0x00abcdef
        }
    }
})

fixture.add_node()

fixture.update_configuration_value(42, 4, 0x001a2b3e)
fixture.check_set_setting_value_called({ red = 0x3e, green = 0x2b, blue = 0x1a }, "synced")

fixture.remove_node()
