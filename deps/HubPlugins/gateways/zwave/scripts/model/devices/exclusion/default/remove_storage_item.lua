local self, item, skip_monitoring_cache = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

if not skip_monitoring_cache then
    local item_names = require("HUB:zwave/scripts/resources/item_names")

    if item.name == item_names.user_codes then
        storage:call("delete_table", storage:call("get_usernames_tablename", item.id) )
    elseif item.name == item_names.master_code then
        storage:call("remove_value", storage:call("get_master_code_key_name", item.id))
    elseif item.name == item_names.weekly_user_code_intervals or
            item.name == item_names.daily_user_code_intervals then
        local descriptor = storage:call("find_item_descriptor", item.id)
        if descriptor then
            local slots_mapper_name = descriptor.tag or item.name
            local slots_mapper = hubplugins.weak_loadfile("HUB:zwave/scripts/utils/slots_mapper/create")(slots_mapper_name, item.id)
            storage:call("delete_table", slots_mapper:call("get_slots_mappings_table_name") )
            storage:call("delete_table", slots_mapper:call("get_commands_table_name") )
        end
    end
end

storage:call("remove_item_mapping", item.id)
