local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")
local extensions_fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/plugins_extensions_fixture")()

extensions_fixture.init()
extensions_fixture.add_extension_data(
    "test_extension_1", 
    {
        ["0_0_0"] = {
            {
                name = "Integer setting name override",
                description = "Integer setting description override",
                type = "int",
                value_min = 0,
                value_max = 255,
                configuration = {
                    number = 42,
                    size = 2,
                    value = 72
                }
            }
        }
    }
)

extensions_fixture.add_extension_data(
    "test_extension_2", 
    {
        ["0_0_0"] = {
            {
                name = "Integer setting name override 2",
                description = "Integer setting description override 2",
                type = "int",
                value_min = 0,
                value_max = 255,
                configuration = {
                    number = 42,
                    size = 2,
                    value = 73
                }
            }
        }
    }
)

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

fixture.add_node()
fixture.check_add_configuration_called({ 42, 2 }, { 71, 2 })

fixture.check_device_settings({
    fixture.rediscovering_configuration,
    fixture.generic_zwave_configurations,
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_42",
            text = "Integer setting name override"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_42",
            text = "Integer setting description override"
        },
        value_type = "int",
        value_default = 72,
        value_min = 0,
        value_max = 255,
        value = 72,
        info = {
            ["zwave.parameter_number"] = 42,
            ["zwave.parameter_size"] = 2
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_71",
            text = "Integer setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_71",
            text = "Integer setting description"
        },
        value_type = "int",
        value_min = 0,
        value_max = 255,
        value = 0,
        info = {
            ["zwave.parameter_number"] = 71,
            ["zwave.parameter_size"] = 2
        }
    }
})

fixture.remove_node()
