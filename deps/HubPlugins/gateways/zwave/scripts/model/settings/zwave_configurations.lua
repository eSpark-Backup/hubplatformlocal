local node_id = ...

return {
    label = {
        lang_tag = "zwave_device_configuration",
        text = "Z-Wave Configurations"
    },
    description = {
        lang_tag = "zwave_device_configuration_descr",
        text = "Specific Z-Wave configuration parameters"
    },
    value_type = "dictionary.zwave_configuration",
    value = {},
    descriptor = {
        node_id = node_id,
        class_id = require("HUB:zwave/scripts/resources/class_ids").configuration
    }
}
