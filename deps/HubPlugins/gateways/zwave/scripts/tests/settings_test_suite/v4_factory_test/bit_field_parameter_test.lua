local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_configurations_version(4)

fixture.set_configurations_v3_parameters(
    {
        {
            id = 1,
            format = "bit_field",
            size = 1,
            min_value = 0,
            max_value = 11,
            default_value = 2,
            name = "Configuration parameter name 1",
            info = "Configuration parameter info 1"
        },
        {
            id = 256,
            format = "bit_field",
            size = 4,
            min_value = 0,
            max_value = 3328,
            default_value = 2304,
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
            ["0"] = {
                text = "0"
            },
            ["1"] = {
                text = "1"
            },
            ["3"] = {
                text = "3"
            }
        },
        value_type = "array.token",
        value_default = {"1"},
        value = {"1"},
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
            ["8"] = {
                text = "8"
            },
            ["10"] = {
                text = "10"
            },
            ["11"] = {
                text = "11"
            }
        },
        value_type = "array.token",
        value_default = {"8", "11"},
        value = {"8", "11"},
        info = {
            ["zwave.parameter_number"] = 256,
            ["zwave.parameter_size"] = 4
        },
        has_setter = true
    }
})

fixture.remove_node()
