local self, node_id, channel_id, class_id, value, target_value = ...

local dw_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/dw_state_event")
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

local item_value
if value == 0x00 then -- Close
    item_value = dw_enum.dw_is_closed

elseif value == 0xFF then -- Open
    item_value = dw_enum.dw_is_opened

else
    error("[134_258_97.update_value]: unsupported sensor binary value: " .. value)
end

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local item_name = item_names.dw_state
local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_ids.notification, item_descriptor_tags.access_control)

self:call("update_value_in_core", item_name, item_id, item_value)