local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")

local device_filter = {
    keypad = true
}

local user_codes_item = item_factory.create_user_codes_item()
local user_code_action_item = item_factory.create_user_code_action_item()

migration_utils.for_each_device(device_filter, function(device)
    migration_utils.add_item(device.id, user_codes_item)
    migration_utils.add_item(device.id, user_code_action_item)
end)
