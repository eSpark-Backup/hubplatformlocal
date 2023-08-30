local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")
local tests = require("tests")

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

fixture.set_setting_value("value_1")
fixture.check_set_setting_value_called("value_1", "pending")
fixture.check_set_configuration_value_called(42, 1, 0)

fixture.set_setting_value("value_2")
fixture.check_set_setting_value_called("value_2", "pending")
fixture.check_set_configuration_value_called(42, 1, 1)

fixture.set_setting_value("value_3")
fixture.check_set_setting_value_called("value_3", "pending")
fixture.check_set_configuration_value_called(42, 1, 2)

tests.assert_failed(fixture.set_setting_value, "value_4")

fixture.remove_node()
