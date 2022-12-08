local self, device_id = ...

local core = require("core")

local function callback(device)
    if device and device.info and device.info["zwave.smartstart"] == "yes" then
        local smart_start = hubplugins.object( "smart_start", "HUB:zwave/scripts/utils" )
        smart_start:call( "stop_tracking_device_changes", device.id )
    end

    local items = core.get_items_by_device_id(device.id)
    if items then
        for _, item in ipairs(items) do
            self:call("remove_storage_item", item)
        end
    else
        print("[exclusion.remove_node]: items not found for device " .. device.id)
    end

    local setting_ids = core.get_setting_ids_by_device_id(device.id)
    if setting_ids then
        for _, setting_id in pairs(setting_ids) do
            self:call("remove_storage_setting", setting_id)
        end
    else
        print("[exclusion.remove_node]: device settings not found for device " .. device.id)
    end
end

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/for_each_device_by_root_device_id")(device_id, callback)
