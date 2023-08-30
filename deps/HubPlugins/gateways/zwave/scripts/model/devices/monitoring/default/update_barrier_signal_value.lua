local self, node_id, channel_id, class_id, signal_name, value = ...

local core = assert(require("core"), "Failed to load core functionality")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local tag = signal_name

local item_id = self:call("find_item_id", item_names.switch, node_id, channel_id, class_id, tag)

if item_id then
    self:call("update_value_in_core", item_names.switch, item_id, value == 0xFF and true or false)
end
