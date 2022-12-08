local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local storage = require("storage")

local storage_helper = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

info.cc_to_check = {
    [class_ids.door_lock] = true,
    [class_ids.configuration] = true
}

info.storage_setting_deleter = function(setting_id)
    local descriptor = storage.get_table(setting_id)
    local key = storage_helper:call("create_setting_descriptor_key", descriptor.node_id, descriptor.paremeter_number)

    storage.delete(setting_id)
    storage.delete(key)
end

return info
