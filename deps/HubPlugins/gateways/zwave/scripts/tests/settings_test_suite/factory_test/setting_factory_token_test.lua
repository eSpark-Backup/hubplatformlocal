local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

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
                lang_tag = "value_2_tag",
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
    },
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
                lang_tag = "value_2_tag",
                value = 1
            },
            value_3 = {
                text = "Value 3",
                value = 2
            }
        },
        configuration = {
            number = 71,
            size = 1
        }
    }
})

fixture.add_node()
fixture.check_add_configuration_called({ 42, 1 }, { 71, 1 })

fixture.check_device_settings({
    fixture.rediscovering_configuration,
    fixture.generic_zwave_configurations,
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_42",
            text = "Token setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_42",
            text = "Token setting description"
        },
        value_type = "token",
        enum = {
            value_1 = {
                text = "Value 1"
            },
            value_2 = {
                lang_tag = "value_2_tag",
                text = "Value 2"
            },
            value_3 = {
                text = "Value 3"
            },
        },
        value_default = "value_2",
        value = "value_2",
        info = {
            ["zwave.parameter_number"] = 42,
            ["zwave.parameter_size"] = 1
        },
        status = "pending"
    },
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_71",
            text = "Token setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_71",
            text = "Token setting description"
        },
        value_type = "token",
        enum = {
            value_1 = {
                text = "Value 1"
            },
            value_2 = {
                lang_tag = "value_2_tag",
                text = "Value 2"
            },
            value_3 = {
                text = "Value 3"
            },
        },
        value = "unknown",
        info = {
            ["zwave.parameter_number"] = 71,
            ["zwave.parameter_size"] = 1
        }
    }
})

fixture.remove_node()
