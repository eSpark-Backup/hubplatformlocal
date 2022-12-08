local self, node_id, channel_id, class_id, value, target_value, duration = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

if class_id ~= class_ids.switch_binary and class_id ~= class_ids.dimmer then
    self:_super_call(self.base, "update_value", node_id, channel_id, class_id, value, target_value, duration)
    return
end

if channel_id == 0 then
    -- Note: The value of the parent item is based solely on values of child items
    return
end

local parent_switch = self:call("find_item", item_names.switch, node_id, 0, class_id)
local child_switch = self:call("find_item", item_names.switch, node_id, channel_id, class_id)

local is_switch_on = value ~= 0
self:call("update_value_in_core", item_names.switch, child_switch.id, is_switch_on)

if parent_switch.value and not is_switch_on then
    -- Parent was ON and a child item is now OFF -> check if any other child items are still ON
    local channel = self:call("find_channel_with_cc", node_id, class_id, function(channel)
        return channel.id ~= 0 and channel.id ~= channel_id and
               self:call("find_item", item_names.switch, node_id, channel.id, class_id).value == true
    end)

    local any_child_switches_on = channel ~= nil
    self:call("update_value_in_core", item_names.switch, parent_switch.id, any_child_switches_on)
elseif not parent_switch.value and is_switch_on then
    -- Parent was OFF and a child item is now ON -> set parent to ON
    self:call("update_value_in_core", item_names.switch, parent_switch.id, true)
else
    -- Parent was ON and a child is now ON -> do nothing
    -- Parent was OFF and a child is not OFF -> do nothing
end
