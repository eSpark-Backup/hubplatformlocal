local node_id, dev_handler_id, setting_info = ...

local value_mapping = { value = setting_info.value }

local setting = hubplugins.weak_loadfile("HUB:zwave/scripts/model/settings/base")(node_id, dev_handler_id, setting_info, value_mapping)

setting.value_default = nil
setting.value = setting_info.label

return setting
