local self, node_id, dev_handler_id, setting_info = ...

if setting_info.scale ~= nil then
    return hubplugins.weak_loadfile("HUB:zwave/scripts/model/settings/scalable")(node_id, dev_handler_id, setting_info)
else
    local factory = hubplugins.weak_loadfile("HUB:zwave/scripts/model/settings/" .. setting_info.type)
    assert(factory, ("Unknown setting type '%s'"):format(setting_info.type))

    return factory(node_id, dev_handler_id, setting_info)
end
