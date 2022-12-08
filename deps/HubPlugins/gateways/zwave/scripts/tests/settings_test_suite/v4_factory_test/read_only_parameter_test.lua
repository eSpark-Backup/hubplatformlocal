local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

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
            info = "Configuration parameter info 1",
            read_only = true
        },
                {
            id = 256,
            format = "unsigned_integer",
            size = 4,
            min_value = 5,
            max_value = 150350,
            default_value = 10000,
            name = "Configuration parameter name 2",
            info = "Configuration parameter info 2",
            read_only = false
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
        value_type = "int",
        value_default = 10,
        value_min = 5,
        value_max = 15,
        value = 10,
        info = {
            ["zwave.parameter_number"] = 1,
            ["zwave.parameter_size"] = 1
        },
        has_setter = false
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
        value_type = "int",
        value_default = 10000,
        value_min = 5,
        value_max = 150350,
        value = 10000,
        info = {
            ["zwave.parameter_number"] = 256,
            ["zwave.parameter_size"] = 4
        },
        has_setter = true
    }
})

fixture.remove_node()
