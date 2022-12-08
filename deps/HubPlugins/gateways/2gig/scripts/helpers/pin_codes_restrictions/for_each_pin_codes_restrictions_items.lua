local callback = ...

local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local function for_each_device(filter, callback)
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

local device_filter = {
    keypad = true
}

for_each_device(device_filter, function(device)
    local serial = storage_utils.find_serial(device.id)
    if serial then
        local device_items = loadfile("HUB:2gig/scripts/model/devices/keypad")().get_item_ids(serial)
        if  device_items 
        and device_items.user_codes_id 
        and device_items.week_user_code_restriction_id 
        and device_items.day_user_code_restriction_id then        
            local user_codes_item = core.get_item(device_items.user_codes_id)
            local week_user_code_restriction_item = core.get_item(device_items.week_user_code_restriction_id)
            local day_user_code_restriction_item = core.get_item(device_items.day_user_code_restriction_id)
            
            if user_codes_item and week_user_code_restriction_item and day_user_code_restriction_item then
                callback(
                    user_codes_item,
                    week_user_code_restriction_item,
                    day_user_code_restriction_item
                )
            end
        end
    end    
end)
