local params = ...

if not params then
    print("Parametrs is empty")
    return
end

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local zigbee = require("zigbee")

local function set_item_value(item_id)
    local descriptor = storage:call("find_item_descriptor", item_id)
    if not descriptor then
        print( "Item descriptor not found with id: " .. item_id)
        return
    end

    local node = zigbee.get_node(descriptor.node_id)

    local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_monitoring")(node)
    dev_handler:set_item_value(item_id, descriptor, params.value)
end

if params.item_ids then
    for _, item_id in ipairs(params.item_ids) do
        set_item_value(item_id)
    end
elseif params.item_id then
    set_item_value(params.item_id)
end
