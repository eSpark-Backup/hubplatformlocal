local self, item_name, node_id, channel_id, class_id, tag = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

return storage:call("find_item_id", item_name, node_id, class_id, channel_id, tag)
