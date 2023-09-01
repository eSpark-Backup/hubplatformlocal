local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

local door_lock_state = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/door_lock_state")
local to_item_value = {
    not_fully_locked = door_lock_state.unsecured,
    unlocked = door_lock_state.unsecured,
    locked = door_lock_state.secured,
    unknown = door_lock_state.unknown
}

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if attribute.name ~= "LockState" then return end

-- update door lock value
local item_id = handler:find_item_id(item_names.door_lock, node.id, endpoint_id, cluster_name, attribute.name)
if item_id then
    handler:update_value_in_core(item_id, to_item_value[attribute.formatted_value])
end
