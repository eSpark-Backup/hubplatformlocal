local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Integer setting name",
        description = "Integer setting description",
        type = "int",
        value_min = 0,
        value_max = 255,
        configuration = {
            number = 42,
            size = 2,
            value = 71
        }
    },
    {
        name = "Integer setting name",
        description = "Integer setting description",
        type = "int",
        value_min = 0,
        value_max = 255,
        configuration = {
            number = 71,
            size = 2
        }
    }
})

fixture.set_configurations_version(4)

fixture.add_node()

fixture.check_device_settings({
    fixture.rediscovering_configuration
})

fixture.remove_node()
