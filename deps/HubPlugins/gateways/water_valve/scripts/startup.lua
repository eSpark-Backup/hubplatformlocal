print("water_valve: startup")

local water_valve = assert(require "water_valve", "Fail to load water_valve functionality")
local storage = assert(require "storage", "Fail to load storage functionality")
local storage_keys_getter = loadfile("HUB:water_valve/scripts/utils/storage_keys_getter")()
local core = assert(require "core", "Fail to load core functionality")

local function add_device_if_needed(parent_dev_id, main_item_name, index)
    local device_storage_key = storage_keys_getter.get_device_key(main_item_name, index)
    local dev_id = storage.get_string(device_storage_key)

    if dev_id == nil then
        print("water_valve: start device creation " .. device_storage_key)

        dev_id = loadfile("HUB:water_valve/scripts/utils/add_device")(parent_dev_id, main_item_name, index)
        storage.set_string(device_storage_key, dev_id)
    else
        print("water_valve device " .. device_storage_key .. " is already present")
    end

    return dev_id
end

local function add_item_if_needed(dev_id, item_name, index)
    local item_storage_key = storage_keys_getter.get_item_key(item_name, index)
    local item_id = storage.get_string(item_storage_key)

    if item_id == nil then
        item_id = loadfile("HUB:water_valve/scripts/utils/add_item")(dev_id, item_name, index)
        storage.set_string(item_storage_key, item_id)
    end
end

local item_names = loadfile("HUB:water_valve/scripts/resources/item_names")()

local switch_dev_id = add_device_if_needed(nil, item_names.switch)
add_item_if_needed(switch_dev_id, item_names.switch)
add_item_if_needed(switch_dev_id, item_names.water_valve_state)

for index = 0, water_valve.get_water_leak_sensors_count() - 1, 1 do
    local dev_id = add_device_if_needed(switch_dev_id, item_names.water_leak_alarm, index)
    add_item_if_needed(dev_id, item_names.water_leak_alarm, index)
end

water_valve.subscribe( "HUB:water_valve/scripts/events_handling" )
water_valve.request_all_values()
core.update_reachable_state(switch_dev_id,true)
