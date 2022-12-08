local self, device_id = ...

local node_id = assert(self._storage:call("find_node_id", device_id))

local node_info = self._zwave.get_node_info(node_id)
local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(node_info.manufacturer_id, node_info.product_type, node_info.product_id)
local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)

return assert(device_info.thermostat_setpoints_spread)
