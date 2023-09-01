local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Temperature setting name",
        description = "Temperature setting description",
        type = "temperature",
        scale = "celsius",
        value_min = -100,
        value_max = 100,
        configuration = {
            number = 42,
            size = 1,
            value = 71
        }
    },
    {
        name = "Temperature setting name",
        description = "Temperature setting description",
        type = "temperature",
        scale = "celsius",
        value_min = -100,
        value_max = 100,
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
            text = "Temperature setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_42",
            text = "Temperature setting description"
        },
        value_type = "temperature",
        value_default = {
            value = 71,
            scale = "celsius"
        },
        value_min = {
            value = -100,
            scale = "celsius"
        },
        value_max = {
            value = 100,
            scale = "celsius"
        },
        value = {
            value = 71,
            scale = "celsius"
        },
        info = {
            ["zwave.parameter_number"] = 42,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_71",
            text = "Temperature setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_71",
            text = "Temperature setting description"
        },
        value_type = "temperature",
        value_min = {
            value = -100,
            scale = "celsius"
        },
        value_max = {
            value = 100,
            scale = "celsius"
        },
        value = {
            value = 0,
            scale = "celsius"
        },
        info = {
            ["zwave.parameter_number"] = 71,
            ["zwave.parameter_size"] = 1
        }
    }
})

fixture.remove_node()
