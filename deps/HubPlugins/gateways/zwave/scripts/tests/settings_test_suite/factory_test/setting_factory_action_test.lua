local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.set_device_settings({
    {
        name = "Action setting name",
        description = "Action setting description",
        type = "action",
        value = 0xFF,
        label = {
            text = "Button label",
        },
        configuration = {
            number = 42,
            size = 1,
            value = 0
        }
    },
    {
        name = "Action setting name",
        description = "Action setting description",
        type = "action",
        value = 0xFF,
        label = {
            text = "Button label",
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
            text = "Action setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_42",
            text = "Action setting description"
        },
        value_type = "action",
        value = {
            text = "Button label"
        },
        info = {
            ["zwave.parameter_number"] = 42,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_0_0_0_71",
            text = "Action setting name"
        },
        description = {
            lang_tag = "zwave_config_descr_0_0_0_71",
            text = "Action setting description"
        },
        value_type = "action",
        value = {
            text = "Button label"
        },
        info = {
            ["zwave.parameter_number"] = 71,
            ["zwave.parameter_size"] = 1
        }
    }
})

fixture.remove_node()
