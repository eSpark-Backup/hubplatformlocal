local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local setting_factory = require("HUB:2gig/scripts/helpers/setting_factory")

local model = "2GIG-DW10-345"

local device_filter = {
    door_window_sensor = true
}

migration_utils.for_each_device(device_filter, function(device)
    if device.device_type_id ~= model then
        return
    end

    local setting_infos = loadfile("HUB:2gig/scripts/model/devices/custom_settings/" .. model)()
    for _, setting_info in ipairs(setting_infos) do
        local setting = setting_factory.create_token_setting(setting_info)
        migration_utils.add_setting(device.id, setting)
    end
end)
