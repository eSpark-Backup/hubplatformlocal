local params = ...

local storage = assert(require "storage", "Fail to load storage functionality")
local core = assert(require "core", "Fail to load core functionality")
local storage_keys_getter = loadfile("HUB:water_valve/scripts/utils/storage_keys_getter")()
local should_invert_control = loadfile("HUB:water_valve/scripts/utils/should_invert_control")

local function update_value(item_name, value, index)
    local item_id = storage.get_string( storage_keys_getter.get_item_key(item_name, index) )
    if item_id ~= nil then
        core.update_item_value(item_id, value)
    else
        error("water_valve: cannot find item id for item name " .. item_name)
    end
end

local item_names = loadfile("HUB:water_valve/scripts/resources/item_names")()

if params.event == "valve_switch_value_updated" then
    local switch_value =  params.value
    if should_invert_control() then switch_value = not switch_value end
    update_value(item_names.switch, switch_value)

    local valve_state_token_value = params.value == true and "valve_is_opened" or "valve_is_closed"
    update_value(item_names.water_valve_state, valve_state_token_value)

elseif params.event == "water_leak_value_updated" then
    local token_value = params.value == true and "water_leak_detected" or "no_water_leak"
    update_value(item_names.water_leak_alarm, token_value, params.sensor_index)
else
    error("water_valve: unknown event " .. params.event)
end
