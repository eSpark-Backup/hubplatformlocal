local self, node_id, device_id, check, items = ...

local zwave = require("zwave")
local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local existing_checking_device = storage:call("find_device_checking_id", node_id)
if existing_checking_device then
    print("Already checking node: " .. node_id)
    core.send_device_checked_broadcast({device_id = device_id, status = "failed"})
    return
else
    storage:call("insert_device_checking_id", node_id, device_id)
end


if check == "status" then
    zwave.check_reachable_status(node_id)
    return
end

if not items then
    local device_items = core.get_items_by_device_id(device_id)
    items = {}
    for _, device_item in ipairs(device_items) do
        table.insert(items, device_item.id)
    end
    
    device_items = nil
    hubplugins.try_collectgarbage()    
end

local cc_to_check = {}
local unique_table = {}
for _, item_id in ipairs(items) do
    local descriptors = storage:call("find_item_descriptors", item_id)
    for _, descriptor in ipairs(descriptors) do
        local unique_description = ("%d_%d"):format(math.floor(descriptor.channel_id), math.floor(descriptor.class_id))
        if not unique_table[unique_description] then
            unique_table[unique_description] = true
            table.insert(cc_to_check, {channel_id = descriptor.channel_id, class_id = descriptor.class_id})
        end
    end
end

local node_info = zwave.get_node_info(node_id)

local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(
    node_info.manufacturer_id,
    node_info.product_type,
    node_info.product_id
)

local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)

zwave.check_node_values(node_id, device_info.request_value_delay, cc_to_check)
