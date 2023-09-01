local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")

local device_filter = {
    keypad = true
}

local week_user_code_restriction_item = item_factory.create_week_user_code_restriction_item()
local day_user_code_restriction_item = item_factory.create_day_user_code_restriction_item()

migration_utils.for_each_device(device_filter, function(device)
    migration_utils.add_item(device.id, week_user_code_restriction_item)
    migration_utils.add_item(device.id, day_user_code_restriction_item)
end)
