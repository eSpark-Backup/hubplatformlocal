local callback = ...

local core = require("core")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local function for_each_device(callback)
    local devices = core.get_devices()
    local gateway_id = core.get_gateway().id

    if devices ~= nil then
        for _, device in ipairs(devices) do
            if device.gateway_id == gateway_id then
                callback(device)
            end
        end
    end
end

local function find_item(items, item_name, check_cc_door_lock)
    local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
    local _,item = find_if(items, function(item) return item.name == item_name end)
    if item and check_cc_door_lock then
        local storage = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils" )
        local item_descriptors = storage:call("find_item_descriptors", item.id)
        if #item_descriptors ~= 1 or item_descriptors[1].class_id ~= class_ids.user_codes then
            return nil
        end
    end

    return item
end

for_each_device(function(device)
    local items = core.get_items_by_device_id(device.id)

    local user_codes_item = find_item(items, item_names.user_codes, false)
    local week_user_code_restriction_item = find_item(items, item_names.weekly_user_code_intervals, true)
    local day_user_code_restriction_item = find_item(items, item_names.daily_user_code_intervals, true)

    if user_codes_item and week_user_code_restriction_item and day_user_code_restriction_item then
        callback(
            user_codes_item,
            week_user_code_restriction_item,
            day_user_code_restriction_item
        )
    end
end)
