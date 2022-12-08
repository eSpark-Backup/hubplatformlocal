local device, core_device_id = ...

local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local items = device.create_items()
local item_lookup_keys = {}

for _, item in ipairs(items) do
    item.device_id = core_device_id

    local core_item_id = core.add_item(item)

    item_lookup_keys[core_item_id] = { item.name, item.tag }
end

storage_utils.register_items(core_device_id, item_lookup_keys)
