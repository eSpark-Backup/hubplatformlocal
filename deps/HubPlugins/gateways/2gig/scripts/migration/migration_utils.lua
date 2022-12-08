local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local utils = {}

utils.for_each_device = function(filter, callback)
    local devices = core.get_devices()
    local gateway_id = core.get_gateway().id

    if devices ~= nil then
        for _, device in ipairs(devices) do
            if device.gateway_id == gateway_id then
                local device_type
                if device.info == nil then
                    device_type = device.device_type_id
                else
                    local device_info = require("HUB:2gig/scripts/model/devices/generated/" .. device.device_type_id)
                    device_type = device_info.device_type
                end
    
                if filter[device_type] ~= nil then
                    callback(device)
                end
            end
        end
    end
end

utils.remove_item = function(device_id, item_name, item_tag)
    local old_item_id = storage_utils.find_item_id(device_id, item_name, item_tag)
    core.remove_item(old_item_id)
    storage_utils.unregister_item(device_id, item_name, item_tag)
end

utils.add_item = function(device_id, item)
    item.device_id = device_id
    local core_item_id = core.add_item(item)
    storage_utils.register_item(device_id, core_item_id, item.name)
end

return utils
