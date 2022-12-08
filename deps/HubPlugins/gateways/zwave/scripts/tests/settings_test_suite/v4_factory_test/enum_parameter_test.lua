local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_configurations_version(4)

fixture.set_configurations_v3_parameters(
    {
        {
            id = 1,
            format = "enumerated",
            size = 1,
            min_value = 1,
            max_value = 4,
            default_value = 3,
            name = "Configuration parameter name 1",
            info = "Configuration parameter info 1"
        },
        {
            id = 256,
            format = "enumerated",
            size = 4,
            min_value = 256,
            max_value = 258,
            default_value = 257,
            name = "Configuration parameter name 2",
            info = "Configuration parameter info 2"
        },
    }
)

fixture.add_node()

fixture.check_device_settings({
    fixture.rediscovering_configuration,
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_1",
            text = "Configuration parameter name 1"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_1",
            text = "Configuration parameter info 1"
        },
        enum = {
            ["1"] = {
                text = "1"
            },
            ["2"] = {
                text = "2"
            },
            ["3"] = {
                text = "3"
            },
            ["4"] = {
                text = "4"
            },
        },
        value_type = "token",
        value_default = "3",
        value = "3",
        info = {
            ["zwave.parameter_number"] = 1,
            ["zwave.parameter_size"] = 1
        },
        has_setter = true
    },
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_256",
            text = "Configuration parameter name 2"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_256",
            text = "Configuration parameter info 2"
        },
        enum = {
            ["256"] = {
                text = "256"
            },
            ["257"] = {
                text = "257"
            },
            ["258"] = {
                text = "258"
            },
        },
        value_type = "token",
        value_default = "257",
        value = "257",
        info = {
            ["zwave.parameter_number"] = 256,
            ["zwave.parameter_size"] = 4
        },
        has_setter = true
    }
})

fixture.remove_node()
