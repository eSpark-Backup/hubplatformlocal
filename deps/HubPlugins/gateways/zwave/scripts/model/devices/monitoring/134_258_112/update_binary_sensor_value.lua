local self, node_id, channel_id, class_id, sensor_type, value = ...

local dw_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/dw_state_event")
local result_value = value == 0x00 and dw_enum.dw_is_closed or dw_enum.dw_is_opened

local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_name =  item_names.dw_state
local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id, item_tags.access_control)

self:call("update_value_in_core", item_name, item_id, result_value)
