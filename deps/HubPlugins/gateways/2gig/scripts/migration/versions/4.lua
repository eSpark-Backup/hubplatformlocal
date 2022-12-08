local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local add_device = loadfile("HUB:2gig/scripts/add_device")
local remove_device = loadfile("HUB:2gig/scripts/remove_device")

local model = "2GIG-FF-345"

local device_filter = {
    smoke_detector = true
}

migration_utils.for_each_device(device_filter, function(device)
    if device.device_type_id ~= model then
        return
    end

    local serial = storage_utils.serial_by_device_id(device.id)

    remove_device({
        deviceId = device.id
    })

    add_device({
        serial = serial,
        model = model
    })
end)
