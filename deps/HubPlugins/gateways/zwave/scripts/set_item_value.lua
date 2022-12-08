local params = ...
if not params then
    print("Parametrs is empty")
    return
end

require("HUB:zwave/scripts/helpers/scripts_preload")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local item_ids = params.item_ids
local item_id = params.item_id
local value = params.value

assert(item_ids or item_id, "One of parameters 'item_id' and 'item_ids' should be specified")

local is_scalable_value = (type(value) == "table") and value.scale ~= nil
if is_scalable_value then
    local core = require("core")
    local item = core.get_item(item_id or item_ids[1])
    local previous_value = item.value

    local unit_converter = hubplugins.object("unit_converter", "HUB:zwave/scripts/utils")

    if previous_value.scale ~= value.scale then
       value = {
             value = unit_converter:call(item.value.relative and "convert_offset" or "convert", value.value, value.scale, previous_value.scale),  -- temporary solution. use hub's scale settings after has been implemented
             scale = previous_value.scale
       }
    end
end

if item_ids ~= nil then
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value_multicast")({
        item_ids = item_ids,
        value = value
    })
    return
end

local descriptor = storage:call("find_item_descriptor", item_id)
if not descriptor then
    print( "Item descriptor not found with id: " .. item_id)

    return
end

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(descriptor.node_id)
dev_handler:call("set_item_value", item_id, descriptor, value)
