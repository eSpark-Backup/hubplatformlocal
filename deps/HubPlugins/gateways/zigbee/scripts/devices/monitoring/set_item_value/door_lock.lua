local handler, item_id, descriptor, value = ...

local zigbee = require("zigbee")
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local door_lock_state = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/door_lock_state")

if descriptor.item_name ~= item_names.door_lock then return end

local command_name
if value == door_lock_state.unsecured then
    command_name = "UnlockDoor"
elseif value == door_lock_state.secured then
    command_name = "LockDoor"
end

if command_name then
    zigbee.send_command( 
    descriptor.node_id,
    descriptor.endpoint_id, 
    descriptor.cluster_name, 
    command_name
)
end
