local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local storage = require("storage")

info.check_all_nodes = true

info.storage_item_deleter = function(item)
    print("info.storage_item_deleter")
    local descriptor = storage.get_table(item.id)
    if not descriptor then
        print("[migration.storage_item_deleter] item_id=" .. item.id .. " not found!")
        return
    end

    local storage_helper = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

    if descriptor[1] then
        storage_helper:call("remove_item_mapping", item.id)
    elseif descriptor.item_name and descriptor.node_id and descriptor.class_id and descriptor.channel_id then
        local key =
            storage_helper:call("create_item_descriptor_key",
                descriptor.item_name,
                descriptor.node_id,
                descriptor.class_id,
                descriptor.channel_id,
                descriptor.tag
            )
    
        storage.delete(key)
        storage.delete(item.id)
    end
end

info.items_old_names = {
    ["alarm_co"] = "co_alarm",
    ["alarm_co2"] = "co2_alarm",
    ["alarm_intrusion_tamper"] = "tampering_cover_alarm",
    ["alarm_smoke"] = "smoke_alarm"
}

return info
