local params = ...

require ("HUB:plughub/scripts/helpers/scripts_preload")

local storage = assert(require "storage", "Fail to load storage functionality")
local core = assert(require "core", "Fail to load core functionality")

local item_storage_prefix = plughubplugins.weak_require("HUB:plughub/scripts/resources/item_storage_prefix")

local function update_value(item_name, value)
    if value == nil then
        print("Value is nil for item " .. item_name)
        return
    end

    local item_id = storage.get_string(item_storage_prefix .. item_name)
    if item_id then
        core.update_item_value(item_id, value)
    else
        print("Cannot find item id for item name " .. item_name)
    end
end

if params.event == "switch_value_updated" then
    update_value("switch", params.value)

elseif params.event == "electric_meter_values_updated" then
    local meters = plughubplugins.weak_require("HUB:plughub/scripts/resources/electric_meter_info")
    for meter, item_name in pairs(meters.meter_to_item_name) do
        if params[meter] then
            update_value(
                item_name,
                {
                    value = params[meter],
                    scale = meters.meter_to_scale[meter]
                }
            )
        end
    end

elseif params.event == "product_name_updated" then
    print("Product name updated")

    local default_device = plughubplugins.weak_require("HUB:plughub/scripts/device/device_info")
    local product_name = core.get_product_name()
    local device_storage_key = "plughub_devid"
    local dev_id = storage.get_string(device_storage_key)

    if dev_id then
        local current_device = core.get_device(dev_id)
        if not current_device then
            return
        end

        if not product_name or product_name == '' then
            return
        end

        if (current_device.name ==  default_device.name) then
            current_device.name = product_name
            core.modify_device(dev_id, current_device)
        end
    end
else
    print("Unknown event " .. params.event)
end
