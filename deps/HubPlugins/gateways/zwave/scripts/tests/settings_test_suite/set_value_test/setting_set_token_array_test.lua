local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")
local tests = require("tests")

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

fixture.set_setting_value({ "value_1", "value_2" })
fixture.check_set_setting_value_called({ "value_1", "value_2" }, "pending")
fixture.check_set_configuration_value_called(42, 1, 3)

fixture.set_setting_value({ "value_2", "value_3" })
fixture.check_set_setting_value_called({ "value_2", "value_3" }, "pending")
fixture.check_set_configuration_value_called(42, 1, 6)

fixture.set_setting_value({ "value_3" })
fixture.check_set_setting_value_called({ "value_3" }, "pending")
fixture.check_set_configuration_value_called(42, 1, 4)

tests.assert_failed(fixture.set_setting_value, "value_4")

fixture.remove_node()
