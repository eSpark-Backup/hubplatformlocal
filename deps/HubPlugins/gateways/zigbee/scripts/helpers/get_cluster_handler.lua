local device_type_id, cluster_name = ...

local base_handler_path = "HUB:zigbee/scripts/devices/clusters/"
local handler_path =
    hubplugins.weak_loadfile(base_handler_path .. device_type_id .. "/" .. cluster_name) or
    hubplugins.weak_loadfile(base_handler_path .. cluster_name)

if handler_path then return handler_path() end
