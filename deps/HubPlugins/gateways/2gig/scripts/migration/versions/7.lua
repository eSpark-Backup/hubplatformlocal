local add_settings = loadfile("HUB:2gig/scripts/helpers/add_settings")
local get_device_info = loadfile("HUB:2gig/scripts/helpers/get_device_info")
local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local device_filter = {
    keypad = true
}

migration_utils.for_each_device(device_filter, function(device)
    local tx_id = math.floor(storage_utils.find_serial(device.id))

    local device_info = get_device_info(device.info and device.device_type_id, "keypad")

    add_settings(device_info, device.id, tx_id)
end)
