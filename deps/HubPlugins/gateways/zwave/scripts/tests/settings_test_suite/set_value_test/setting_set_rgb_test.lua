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

fixture.set_setting_value({ red = 0x1a })
fixture.check_set_setting_value_called({ red = 0x1a }, "pending")
fixture.check_set_configuration_value_called(42, 4, 0x0000001a)

fixture.set_setting_value({ green = 0x2b })
fixture.check_set_setting_value_called({ green = 0x2b }, "pending")
fixture.check_set_configuration_value_called(42, 4, 0x00002b00)

fixture.set_setting_value({ blue = 0x3c })
fixture.check_set_setting_value_called({ blue = 0x3c }, "pending")
fixture.check_set_configuration_value_called(42, 4, 0x003c0000)

fixture.set_setting_value({ red = 0x1a, green = 0x2b, blue = 0x3c })
fixture.check_set_setting_value_called({ red = 0x1a, green = 0x2b, blue = 0x3c }, "pending")
fixture.check_set_configuration_value_called(42, 4, 0x003c2b1a)

fixture.remove_node()
