local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")
local extensions_fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/plugins_extensions_fixture")()

extensions_fixture.init()
extensions_fixture.add_extension_data(
    "test_extension_1", 
    {
        ["0_0_0"] = {
            {
                name = "Token setting name extension 1",
                description = "Token setting description extension 1",
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
            }
        }
    }
)

extensions_fixture.add_extension_data(
    "test_extension_2", 
    {
        ["0_0_0"] = {
            {
                name = "Token setting name extension 2",
                description = "Token setting description extension 2",
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
                    value = 2
                }
            }
        }
    }
)

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
            value = 0
        }
    }
})

fixture.set_configurations_version(4)

fixture.set_configurations_v3_parameters(
    {
        {
            id = 42,
            format = "unsigned_integer",
            size = 2,
            min_value = 5,
            max_value = 15,
            default_value = 10,
            name = "Configuration parameter name 1",
            info = "Configuration parameter info 1"
        },
    }
)

fixture.add_node()

fixture.check_device_settings({
    fixture.rediscovering_configuration,
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_42",
            text = "Token setting name extension 1"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_42",
            text = "Token setting description extension 1"
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
    }
})

fixture.remove_node()
