local self, inclusion_context, build_tree_context, device_node = ...
local cc = build_tree_context.command_class
local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local user_code_item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.user_codes)

local items = {
    user_code_item,
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.user_codes_scan_progress)
}

user_code_item.elements_max_number = assert(self:call("get_elements_max_number", cc, device_node_data), "No user code max number.")
user_code_item.user_code_restriction = device_node_data.info.user_code_restriction

local default_user_code_restriction = "^\\d{4,10}$"

if cc.capabilities then
    print("User code checksum " .. ( cc.capabilities.checksum_supported and "supported" or "not supported" ) )

    user_code_item.user_code_restriction = user_code_item.user_code_restriction or
                                           cc.capabilities.supported_keys and cc.capabilities.supported_keys:len() > 0 and ("^[%s]{4,10}$"):format(cc.capabilities.supported_keys) or
                                           default_user_code_restriction

    if cc.capabilities.master_code_supported then
        local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.master_code)
        item.string_restriction = user_code_item.user_code_restriction
        table.insert(items, item)
    end

    if cc.capabilities.master_code_supported and cc.capabilities.master_code_deactivation_supported then
        local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.master_code_state)
        table.insert(items, item)
    end

    if cc.capabilities.supported_keypad_modes and #cc.capabilities.supported_keypad_modes > 1 then
        local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.user_codes_keypad_mode)
        for _, mode in ipairs(cc.capabilities.supported_keypad_modes) do
            table.insert(item.enum, mode)
        end

        table.insert(items, item)
    end

    if not device_node_data.info.ignore_user_code_modes then
        user_code_item.user_code_modes = cc.capabilities.supported_user_id_statuses
    end
else
    user_code_item.user_code_restriction = user_code_item.user_code_restriction or default_user_code_restriction
end

if user_code_item.user_code_modes then
    local remove_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/remove_from_array_if")
    remove_if(user_code_item.user_code_modes, function(mode)
        return mode == "not_available" or mode == "available"
    end)

    if user_code_item.user_code_modes[1] == nil then
        table.insert(user_code_item.user_code_modes, "enabled")
    end
elseif not device_node_data.info.ignore_user_code_modes then
    user_code_item.user_code_modes = { "enabled", "disabled" }
end

-- Process virtual pin codes restrictions
if hubplugins.virtual_pin_codes_restrictions_enabled then
    local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
    local _, disabled_mode = find_if(user_code_item.user_code_modes, function(mode) return mode == "disabled" end)
    local _, cc_schedule = find_if(build_tree_context.channel.classes, function(cc_schedule) return cc_schedule.id == class_ids.schedule_entry_lock end)
    if disabled_mode and not cc_schedule then
        local week_intervals_item = self:call("create_item",
            inclusion_context,
            build_tree_context,
            device_node,
            item_names.weekly_user_code_intervals)

        week_intervals_item.elements_max_number_2 = 30
        table.insert(items, week_intervals_item)

        local day_intervals_item = self:call("create_item",
            inclusion_context,
            build_tree_context,
            device_node,
            item_names.daily_user_code_intervals)

        day_intervals_item.elements_max_number_2 = 30
        table.insert(items, day_intervals_item)
    end
end

self:call("add_simple_items", inclusion_context, build_tree_context, device_node, items)
