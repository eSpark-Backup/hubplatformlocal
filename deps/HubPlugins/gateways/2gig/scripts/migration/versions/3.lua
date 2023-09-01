local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")

local device_filter = {
    tilt_detector = true
}

local item = item_factory.create_door_window_state_item()

migration_utils.for_each_device(device_filter, function(device)
    migration_utils.remove_item(device.id, "tampering_move_alarm")

    migration_utils.add_item(device.id, item)
end)
