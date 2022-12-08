local self, item_descriptor, state = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local zwave = require( "zwave" )

local master_code_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/master_code_state")

if state == master_code_enum.disabled then
    zwave.set_master_code(item_descriptor.node_id, item_descriptor.channel_id, "")
    
elseif state == master_code_enum.enabled then
    local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
    local master_code_item_id = storage:call("find_item_id", 
                                            item_names.master_code, 
                                            item_descriptor.node_id, 
                                            item_descriptor.class_id,
                                            item_descriptor.channel_id)

    assert(master_code_item_id)

    local master_code = storage:call("get_string", storage:call("get_master_code_key_name", master_code_item_id) )

    if master_code then
        zwave.set_master_code(item_descriptor.node_id, item_descriptor.channel_id, master_code)
    else
        error("Cannot enable master code: previous code is unknown!")
    end
else
    error(("Unknown master code state: %s"):format(state))
end

zwave.request_master_code(item_descriptor.node_id, item_descriptor.channel_id)
