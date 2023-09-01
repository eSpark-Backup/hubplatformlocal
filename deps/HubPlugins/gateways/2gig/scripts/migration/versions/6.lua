local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")

local device_filter = {
    smoke_co_detector = true,
    audio_detector = true
}

local item = item_factory.create_security_threat_item()

migration_utils.for_each_device(device_filter, function(device)
    if device.parent_device_id ~= "" then
        migration_utils.add_item(device.id, item)
    end
end)
