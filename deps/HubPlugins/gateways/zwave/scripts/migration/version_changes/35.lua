local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

info.cc_to_check = {
    [class_ids.door_lock] = true
}

local orig_storage_setting_deleter = info.storage_setting_deleter
info.storage_setting_deleter = function(setting_id)
    local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
    local descriptor = storage:call("find_setting_descriptor", setting_id)

    if descriptor.class_id == class_ids.door_lock then
        storage:call("delete_table",  "door_lock_settings_" ..  descriptor.node_id)
    end
    orig_storage_setting_deleter(setting_id)
end

return info