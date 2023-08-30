local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")

local device_filter = {
    panic_button = true
}

local item = item_factory.create_security_threat_item()

migration_utils.for_each_device(device_filter, function(device)
    migration_utils.add_item(device.id, item)
end)
