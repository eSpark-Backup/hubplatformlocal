local self, inclusion_context = ...

local core = require("core")
local create_device_type_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")

local zwave_node = inclusion_context.zwave_node
local dev_info = self.device_info

dev_info.gateway_id = core.get_gateway().id
dev_info.battery_powered = zwave_node.is_battery_powered
dev_info.device_type_id = create_device_type_id(zwave_node.manufacturer_id, zwave_node.product_type, zwave_node.product_id)
dev_info.info.protocol = "zwave"
dev_info.info[ "zwave.smartstart" ] = zwave_node.is_smart_include and "yes" or "no"
dev_info.info[ "zwave.node" ] = zwave_node.id

-- Node ID can only be 1 in case if the node is a primary controller. Primary controller devices are persistent.
if tonumber(zwave_node.id) == 1 then
    dev_info.persistent = true
end

local device_tree_model = hubplugins.object("device_tree_model", "HUB:zwave/scripts/model/devices", dev_info)
device_tree_model:call("insert_child") -- insert root node

if dev_info.panic_triggers and dev_info.panic_button_policy == "separate_device" then
    local panic_button_info = assert(
        hubplugins.weak_loadfile("HUB:zwave/scripts/resources/child_devices_info/panic_button")()
    )
    
    device_tree_model:call("insert_child", device_tree_model:call("get_root_node"), "panic_button", panic_button_info)
end 

return device_tree_model
