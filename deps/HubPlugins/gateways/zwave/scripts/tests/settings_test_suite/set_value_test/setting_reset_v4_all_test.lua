local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")
local tests = require("tests")
local core = require("core")
local zwave = require("zwave")

fixture.set_configurations_version(4)

fixture.set_configurations_v3_parameters(
    {
        {
            id = 1,
            format = "unsigned_integer",
            size = 1,
            min_value = 5,
            max_value = 15,
            default_value = 10,
            name = "Configuration parameter name 1",
            info = "Configuration parameter info 1"
        },
                {
            id = 256,
            format = "unsigned_integer",
            size = 4,
            min_value = 5,
            max_value = 150350,
            default_value = 10000,
            name = "Configuration parameter name 2",
            info = "Configuration parameter info 2"
        },
    }
)

fixture.add_node()

fixture.set_device_settings({
    {
        name = "Boolean setting name",
        description = "Boolean setting description",
        type = "int",
        configuration = {
            number = 1,
            size = 1,
            value = 11
        }
    }
})

zwave.request_node_values = tests.create_mock()

fixture.reset_settings()
fixture.check_reset_all_configuration_values_called()

tests.assert_called(zwave.request_node_values, "check calls to zwave.request_node_values", {
    { fixture.node_id, 200, 5 }
})

fixture.remove_node()
