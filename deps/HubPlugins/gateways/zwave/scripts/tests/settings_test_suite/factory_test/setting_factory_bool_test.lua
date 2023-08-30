local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Boolean setting name",
        description = "Boolean setting description",
        type = "bool",
        configuration = {
            number = 42,
            size = 1,
            value = 0
        }
    },
    {
        name = "Boolean setting name",
        description = "Boolean setting description",
        type = "bool",
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
            text = "Boolean setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_42",
            text = "Boolean setting description"
        },
        value_type = "bool",
        value_default = false,
        value = false,
        info = {
            ["zwave.parameter_number"] = 42,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_71",
            text = "Boolean setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_71",
            text = "Boolean setting description"
        },
        value_type = "bool",
        value = false,
        info = {
            ["zwave.parameter_number"] = 71,
            ["zwave.parameter_size"] = 1
        }
    }
})

fixture.remove_node()
