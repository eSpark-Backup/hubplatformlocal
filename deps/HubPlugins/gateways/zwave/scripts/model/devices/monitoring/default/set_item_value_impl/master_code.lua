local self, item_descriptor, code = ...

assert(code and code ~= "", "Please deactivate master code via item 'master_code_state' instead of passing empty string")

local item_names = require("HUB:zwave/scripts/resources/item_names")

local zwave = require( "zwave" )

local master_code_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/master_code_state")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local master_code_state_item_id = storage:call("find_item_id", 
                                                item_names.master_code_state, 
                                                item_descriptor.node_id, 
                                                item_descriptor.class_id,
                                                item_descriptor.channel_id)

if master_code_state_item_id then
    local master_code_item_id = storage:call("find_item_id", 
                                            item_names.master_code, 
                                            item_descriptor.node_id, 
                                            item_descriptor.class_id,
                                            item_descriptor.channel_id)

    local core = require "core"
    local master_code_state_item = core.get_item(master_code_state_item_id)

    if master_code_state_item.value == master_code_enum.disabled then
        storage:call("set_string", storage:call("get_master_code_key_name", master_code_item_id), code)
        self:call("update_value_in_core", item_names.master_code, master_code_item_id, code)
        return
    end
end

zwave.set_master_code(item_descriptor.node_id, item_descriptor.channel_id, code)
zwave.request_master_code(item_descriptor.node_id, item_descriptor.channel_id)
