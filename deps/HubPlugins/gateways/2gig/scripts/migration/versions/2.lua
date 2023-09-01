local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")

local device_filter = {
    co_detector = true,
    door_window_sensor = true,
    flood_temperature = true,
    glass_break_detector = true,
    pir_detector = true,
    smoke_detector = true,
    tilt_detector = true
}

local item = item_factory.create_security_threat_item()

migration_utils.for_each_device(device_filter, function(device)
    migration_utils.add_item(device.id, item)
end)
