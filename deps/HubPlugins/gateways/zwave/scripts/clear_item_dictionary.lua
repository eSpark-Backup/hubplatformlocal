local params = ...
if not params then
    print("Parametrs is empty")
    return
end

print("clear dictionary")

require ("HUB:zwave/scripts/helpers/scripts_preload")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local item_id = params.item_id
local descriptor = storage:call("find_item_descriptor", item_id)
if not descriptor then
    print( "Item descriptor not found with id: " .. item_id)
    return
end

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(descriptor.node_id)
dev_handler:call("clear_item_dictionary", item_id, descriptor)
