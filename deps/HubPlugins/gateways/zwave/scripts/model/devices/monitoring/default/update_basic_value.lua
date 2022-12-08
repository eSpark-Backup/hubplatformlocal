local self, node_id, channel_id, class_id, value, target_value = ...

local zwave = require("zwave")
local zwave_node = assert( zwave.get_node_info(node_id), "Cannot find zwave node with id " .. node_id )

local cc_data = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_basic_cc_data_by_device_info")( zwave_node )

if not self:call("handle_cc_data", cc_data, node_id, channel_id, value, target_value) then
    local item_names = require("HUB:zwave/scripts/resources/item_names")

    local item_id = self:call("find_item_id", item_names.basic, node_id, channel_id, class_id)
    if not item_id then
        return
    end

    local core = require("core")
    local basic_item = assert(core.get_item(item_id))

    cc_data = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_basic_cc_data_by_basic_item")( basic_item )
    if not self:call("handle_cc_data", cc_data, node_id, channel_id, value ) then
        self:call("update_value_in_core", basic_item.name, basic_item.id, value ~= 0 or value == true )
    end
end
