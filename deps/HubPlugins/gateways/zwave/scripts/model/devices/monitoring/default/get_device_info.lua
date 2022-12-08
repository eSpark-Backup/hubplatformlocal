local self = ...

if self._device_info == nil and  self._node_id ~= nil then
    local zwave = require "zwave"
    local node_info = zwave.get_node_info(self._node_id)
    local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(node_info.manufacturer_id, node_info.product_type, node_info.product_id)
    self._device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)
end

return self._device_info