local self, item_id, item_descriptor, key = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local zwave = require("zwave")
local core = require("core")


if item_descriptor.item_name == item_names.user_codes then
    storage:call("remove_table_value", storage:call("get_usernames_tablename", item_id), key)
    zwave.set_user_code(item_descriptor.node_id, item_descriptor.channel_id, tonumber(key), "available", "")

    local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", item_id, item_descriptor.node_id, item_descriptor.channel_id)
    user_code_requests_sender:call("add_postponed_requests", { key } )

elseif item_descriptor.item_name == item_names.weekly_user_code_intervals
    or item_descriptor.item_name == item_names.daily_user_code_intervals then
        if item_descriptor.class_id == class_ids.user_codes then
        -- Virtual pin codes restrictions, just update value in core
        core.remove_item_dictionary_value(item_id, key)
        hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/process_restrictions")(true)
    else
        local slots_mapper_name = item_descriptor.tag or item_descriptor.item_name
        local slots_mapper = hubplugins.weak_loadfile("HUB:zwave/scripts/utils/slots_mapper/create")(
                                    slots_mapper_name,
                                    item_id,
                                    key,
                                    item_descriptor.node_id,
                                    item_descriptor.channel_id)

        slots_mapper:call("set_values", nil, true)
    end
else
    print("remove_item_dictionary_value: not implemented for dictionary item " .. item_descriptor.item_name)
end
