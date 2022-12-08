local node_id, dev_handler_id, setting_info = ...

local value_mapping = {}
for token_name, token_info in pairs(setting_info.enum) do
    value_mapping[token_name] = token_info.value
    token_info.value = nil

    hubplugins.try_collectgarbage()
end

return hubplugins.weak_loadfile("HUB:zwave/scripts/model/settings/base")(node_id, dev_handler_id, setting_info, value_mapping)
