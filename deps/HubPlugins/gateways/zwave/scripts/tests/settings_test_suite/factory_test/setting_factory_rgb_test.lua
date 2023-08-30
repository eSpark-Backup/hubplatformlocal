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
    },
    {
        name = "RGB setting name",
        description = "RGB setting description",
        type = "rgb",
        components = {
            red = 0,
            blue = 2,
            cyan = 3
        },
        configuration = {
            number = 71,
            size = 4
        }
    }
})

fixture.add_node()
fixture.check_add_configuration_called({ 42, 4 }, { 71, 4 })

fixture.check_device_settings({
    fixture.rediscovering_configuration,
    fixture.generic_zwave_configurations,
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_42",
            text = "RGB setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_42",
            text = "RGB setting description"
        },
        value_type = "rgb",
        value_default = {
            red = 0xef,
            green = 0xcd,
            blue = 0xab
        },
        value = {
            red = 0xef,
            green = 0xcd,
            blue = 0xab
        },
        info = {
            ["zwave.parameter_number"] = 42,
            ["zwave.parameter_size"] = 4
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_71",
            text = "RGB setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_71",
            text = "RGB setting description"
        },
        value_type = "rgb",
        value = {
            red = 0,
            blue = 0,
            cyan = 0
        },
        info = {
            ["zwave.parameter_number"] = 71,
            ["zwave.parameter_size"] = 4
        }
    }
})

fixture.remove_node()
