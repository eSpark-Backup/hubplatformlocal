local setting_factory = {}

function setting_factory.create(setting_name, dev_type_id)
    local setting = nil

    if dev_type_id then
        setting = hubplugins.weak_loadfile(string.format("HUB:zigbee/scripts/devices/settings/%s/%s", dev_type_id, setting_name))
    end

    if setting == nil then
        local setting_script_name = string.format("HUB:zigbee/scripts/devices/settings/%s", setting_name)
        setting = assert(hubplugins.weak_loadfile(setting_script_name), "Setting script not found with name: " .. setting_script_name)
    end

    return setting()
end

return setting_factory
