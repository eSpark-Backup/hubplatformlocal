local self, item_id, item_descriptor, key, value_type, value = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local zwave = require("zwave")
local core = require("core")


if not key then
    if item_descriptor.item_name == item_names.user_codes then
        local cc = zwave.get_command_class(item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)
        assert(cc and cc.supported_users_count, "supported_users_count is not found")

        local device_info = self:call("get_device_info")

        key = tostring(core.generate_item_dictionary_number_id(
            item_id,
            tonumber(device_info.first_user_code or 1),
            tonumber(device_info.end_user_code or cc.supported_users_count)
        ))
    else
        print("Generation of key for dictionary item " .. item_descriptor.item_name .. " is not implemented")
        return
    end
end


if item_descriptor.item_name == item_names.user_codes then
    local cc = zwave.get_command_class(item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)

    local mode_to_set = value.mode or "enabled"

    if mode_to_set ~= "enabled" then
        local device_info = self:call("get_device_info")
        assert(not device_info.ignore_user_code_modes, "Ignoring user code modes. Can't accept mode " .. mode_to_set)

        local supported_statuses = cc.capabilities and cc.capabilities.supported_user_id_statuses or { "disabled", "enabled" }
        local find = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array")

        assert(find(supported_statuses, mode_to_set), "Unknown status: " .. mode_to_set)
    end

    storage:call("set_table_value", storage:call("get_usernames_tablename", item_id), key, value.name )
    zwave.set_user_code(item_descriptor.node_id, item_descriptor.channel_id, tonumber( key ), mode_to_set, value.code)

    local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", item_id, item_descriptor.node_id, item_descriptor.channel_id)
    user_code_requests_sender:call("add_postponed_requests", { key })
elseif item_descriptor.item_name == item_names.weekly_user_code_intervals or
       item_descriptor.item_name == item_names.daily_user_code_intervals then
    if item_descriptor.class_id == class_ids.user_codes then
        -- Virtual pin codes restrictions, just update value in core
        core.update_item_dictionary_value(item_id, key, value)
        hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/process_restrictions")(true)
    else
        local slots_mapper_name = item_descriptor.tag or item_descriptor.item_name
        local slots_mapper = hubplugins.weak_loadfile("HUB:zwave/scripts/utils/slots_mapper/create")(
                                    slots_mapper_name,
                                    item_id,
                                    key,
                                    item_descriptor.node_id,
                                    item_descriptor.channel_id)

        slots_mapper:call("set_values", value, true)
    end
else
    print("set_item_dictionary_value of dictionary item " .. item_descriptor.item_name .. " is not implemented")
end

