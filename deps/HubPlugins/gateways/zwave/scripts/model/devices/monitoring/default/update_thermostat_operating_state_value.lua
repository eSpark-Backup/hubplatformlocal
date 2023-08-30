local self, node_id, channel_id, class_id, value = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local item_id = self:call("find_item_id", item_names.thermostat_operating_state, node_id, channel_id, class_id)
assert(item_id,"[handler.update_thermostat_operating_state_value]: " ..
        self:call("create_item_id_not_found_error_message", item_names.thermostat_operating_state, node_id, channel_id, class_id))

self:call("update_value_in_core", item_names.thermostat_operating_state, item_id, value)
