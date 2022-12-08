local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local descriptor = storage:call("find_item_descriptor", params.item_id)
if not descriptor then
    print( "Item descriptor not found with id: " .. params.item_id)
    return
end

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(descriptor.node_id)

dev_handler:call("set_pin_code_mode", params.item_id, descriptor, params.pin_code_id, params.mode)
