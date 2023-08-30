local core = require('core')
local storage = require('storage')
local logger = require("HUB:siren/scripts/utils/log")

local utils = {}

utils.for_each_device = function(filter, callback)
    local devices = assert(core.get_devices(), "Error loading devices")
    for _, device in ipairs(devices) do
        local is_good = filter[device.device_type_id] ~= nil and filter[device.category] ~= nil
        if is_good then
            callback(device)
        end
    end
end

local function migrate_sirens(device)
    logger.trace('migrate_sirens', 'started')
    if device.house_modes_options == nil then
        local house_modes_options = {
            alarms_off = {"1", "2", "3", "4"}
        }

        device.house_modes_options = house_modes_options
        core.modify_device( device.id, device )
        return true
    end
    return false
end

local migrate_device = function(device)
    local migrationOk = migrate_sirens(device)
    return migrationOk
end

local device_filter = {
    siren = true,
    e550_siren = true
}

local success, error_message = pcall(utils.for_each_device, device_filter, migrate_device)
if success then
    logger.info('Migration: Ok')
    storage.set_number('plugin_version', 2)
else
    logger.error('Migration error: ', error_message)
end
