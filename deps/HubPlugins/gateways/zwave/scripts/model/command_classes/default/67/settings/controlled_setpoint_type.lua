local self, node_id = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

return {
    label = {
        text = "Controlled thermostat setpoint type",
        lang_tag = "controlled_thermostat_setpoint_type"
    },
    description = {
        text = "Controlled thermostat setpoint type",
        lang_tag = "controlled_thermostat_setpoint_type"
    },
    status = "synced",
    value_type = "token",
    enum = {
        default = { text = "Mode dependent (default)", lang_tag = "thermostat_setpoint_type_mode_dependent" }     
    },
    value = "default",
    value_default = "default",
    has_setter = true,
    descriptor = {
        node_id = node_id,
        class_id = class_ids.thermostat_setpoint,
        tag = "controlled_setpoint_type"
    }
}
