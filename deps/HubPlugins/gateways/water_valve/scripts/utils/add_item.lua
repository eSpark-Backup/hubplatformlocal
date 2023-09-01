local dev_id, item_name, index = ...

local core = assert(require "core", "Fail to load core functionality")

local item = loadfile("HUB:water_valve/scripts/items/" .. item_name)()
item.name = item_name
item.device_id = dev_id

local item_id = core.add_item(item)
assert(item_id, "water_valve: Failed to create water_valve item " .. item_name)

return item_id
