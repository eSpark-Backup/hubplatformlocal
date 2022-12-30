local device, core_device_id, tx_id = ...

local twogig = require("2gig")
local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local setting_factory = require("HUB:2gig/scripts/helpers/setting_factory")
local should_configure_device = loadfile("HUB:2gig/scripts/helpers/should_setting_configure_device")

if device.settings == nil then
    return
end

local setting_lookup_keys = {}
local configuration = {}

for _, setting_info in ipairs(device.settings) do
    local setting = setting_factory.create_setting(setting_info.type, setting_info)
    setting.device_id = core_device_id

    local core_setting_id = core.add_setting(setting)

    setting_lookup_keys[core_setting_id] = { setting.info.tag }

    if should_configure_device(setting.info.tag) then
        configuration[setting.info.tag] = setting.value
    end
end

storage_utils.register_settings(core_device_id, setting_lookup_keys)

if next(configuration) then
    twogig.configure_device({ tx_id }, configuration)
end
