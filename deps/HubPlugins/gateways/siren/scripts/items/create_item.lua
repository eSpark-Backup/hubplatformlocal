local dev_id, item_name = ...

local core = assert(require 'core', "Failed to create core module")

local item = loadfile("HUB:siren/scripts/items/" .. item_name)()
item.name = item_name
item.device_id = dev_id

local item_id = core.add_item(item)
assert(item_id, "Failed to create an item " .. item_name)

return item_id
