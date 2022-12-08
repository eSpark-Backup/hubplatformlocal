local self, node_id, channel_id, class_id, code = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local is_active = code and code ~= ""

if is_active then
    local item_id = self:call("find_item_id", item_names.master_code, node_id, channel_id, class_id)
    self:call("update_value_in_core", item_names.master_code, item_id, code)

    local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
    storage:call("set_string", storage:call("get_master_code_key_name", item_id), code)
end

local master_code_state_item_id = self:call("find_item_id", item_names.master_code_state, node_id, channel_id, class_id)

if master_code_state_item_id then
    local master_code_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/master_code_state")

    self:call("update_value_in_core", 
            item_names.master_code_state, 
            master_code_state_item_id, is_active and master_code_enum.enabled or master_code_enum.disabled)
end
