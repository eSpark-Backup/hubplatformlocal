local self, item_descriptor, value = ...

local function is_empty_association(item_value)
    return item_value.deviceId == "" and item_value.partFunction == "unknown"
end


-- main


local item_name = item_descriptor.item_name
local node_id = item_descriptor.node_id
local channel_id = item_descriptor.channel_id
local class_id = item_descriptor.class_id

if not value.deviceId or not value.partFunction then
    print("Incorrect value format")
    return
end

-- check if devices are included to another pair
if not is_empty_association(value) then
    -- check operated device
    local item = self:call("find_item", item_name, node_id, channel_id, class_id)
    if not item then
        print("Failed to find shutter_second_part item")
        return
    end

    if item.value.deviceId ~= "" and item.value.deviceId ~= value.deviceId then
        print("Operated device included to another shades pair.")
        return
    end

    -- check specified device
    local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
    local second_node_id = storage:call("find_node_id", value.deviceId)
    if not second_node_id then
        print("Could node find device " .. value.deviceId)
    end

    local second_item_id = self:call("find_item_id", item_name, second_node_id, channel_id, class_id)
    if not second_item_id then
        print("Specified device is not shutter")
        return
    end
    local second_item = self:call("find_item", item_name, second_node_id, channel_id, class_id)

    local operated_device_id = storage:call("find_device_id", node_id)

    local is_empty_value = is_empty_association(second_item.value)
    local is_correct = second_item.value.deviceId == operated_device_id and second_item.value.partFunction ~= "unknown"
    if not is_empty_value and not is_correct then
        print("Specified device included to another shades pair.")
        return
    end
end

self:call("update_item_value", node_id, channel_id, class_id, item_name, value)
