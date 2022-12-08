local plughub = assert(require "plughub", "Fail to load plughub functionality")
local core = assert(require "core", "Fail to load core functionality")
local storage = assert(require "storage", "Fail to load storage functionality")

require ("HUB:plughub/scripts/helpers/scripts_preload")

local device_storage_key = "plughub_devid"
local item_storage_prefix = plughubplugins.weak_require("HUB:plughub/scripts/resources/item_storage_prefix")

local dev_id = storage.get_string(device_storage_key) 
if not dev_id then
    print("start creation of plughub device")
    local device = plughubplugins.weak_require("HUB:plughub/scripts/device/device_info")
    local product_name = core.get_product_name()

    local gateway = core.get_gateway()
    device.gateway_id = gateway.id
    if product_name and product_name ~= '' then
        device.name = product_name
    end
    dev_id = core.add_device( device )
    assert(dev_id, "Failed to create hubplug device")

    storage.set_string(device_storage_key, dev_id)
else
    print("plughub device is already present")
end

local function add_item_if_needed(item_name)
    local item_id = storage.get_string(item_storage_prefix .. item_name)
    local is_item_added = false

    if not item_id then
        local item = plughubplugins.weak_require("HUB:plughub/scripts/items/" .. item_name)
        item.name = item_name
        item.device_id = dev_id

        item_id = core.add_item(item)
        assert(item_id, "Failed to create hubplug item " .. item_name)
        storage.set_string(item_storage_prefix .. item_name, item_id)
        is_item_added = true
    end

    return is_item_added, item_id
end

add_item_if_needed("switch")
add_item_if_needed("meter_reset")

local meters = plughubplugins.weak_require("HUB:plughub/scripts/resources/electric_meter_info")
for _, item_name in pairs(meters.meter_to_item_name) do
    add_item_if_needed(item_name)

    local is_item_added, item_id = add_item_if_needed(item_name)
    if not is_item_added then
        if item_name == "electric_meter_kwh" then
           local item = core.get_item(item_id)
           plughub.set_consumed_energy_meter(item.value and item.value.value or 0)
        end
   end

end

core.subscribe( "HUB:plughub/scripts/events_handling" )
plughub.subscribe( "HUB:plughub/scripts/events_handling" )
plughub.request_all_values()
core.update_reachable_state(dev_id,true)


