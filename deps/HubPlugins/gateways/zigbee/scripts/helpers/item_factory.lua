local item_factory = {}

function item_factory.create(item_name, dev_type_id)
    local item = nil

    if dev_type_id then
        item = hubplugins.weak_loadfile(string.format("HUB:zigbee/scripts/devices/items/%s/%s", dev_type_id, item_name))
    end

    if item == nil then
        local item_script_name = string.format("HUB:zigbee/scripts/devices/items/%s", item_name)
        item = assert(hubplugins.weak_loadfile(item_script_name), "Item script not found with name: " .. item_script_name)
    end

    return item()
end

return item_factory
