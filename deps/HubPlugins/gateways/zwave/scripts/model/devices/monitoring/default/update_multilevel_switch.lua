local self, node_id, channel_id, class_id, value, target_value, target_duration = ...

local helper = hubplugins.weak_require("HUB:zwave/scripts/helpers/multilevel_switch_helper")
value = helper.value_to_ui(value)
target_value = helper.value_to_ui(target_value)

return self:call("update_dimmer_value", node_id, channel_id, class_id, value, target_value, target_duration)
