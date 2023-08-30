local params = ...

assert(params, "Parametrs are empty")

local storage = assert(require "storage", "Fail to load storage")
local storage_keys_getter = loadfile("HUB:water_valve/scripts/utils/storage_keys_getter")()
local water_valve = assert(require "water_valve", "Fail to load water_valve")
local should_invert_control = loadfile("HUB:water_valve/scripts/utils/should_invert_control")

local item_names = loadfile("HUB:water_valve/scripts/resources/item_names")()

if storage.get_string(storage_keys_getter.get_item_key(item_names.switch)) == params.item_id then
    if should_invert_control() then params.value = not params.value end
    water_valve.set_water_switch_value(params.value)
elseif storage.get_string(storage_keys_getter.get_item_key(item_names.water_valve_state)) == params.item_id then
    print("water_valve: set switch value instead of water_valve_state")
else
    error("water_valve: set_item_value, wrong item id " .. params.item_id)
end
