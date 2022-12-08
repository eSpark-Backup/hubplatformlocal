local self, inclusion_context, build_tree_context, device_node = ...

local cc = build_tree_context.command_class

if cc.week_day_slots == nil and cc.year_day_slots == nil and cc.daily_repeating_slots == nil then
    print("cc schedule entry lock is not supported")
    return
end

print("Inclusion of schedule entry lock." .. 
    "  week_day_slots: " .. cc.week_day_slots .. 
    ", year_day_slots: " .. cc.year_day_slots .. 
    ", daily_repeating_slots: " .. cc.daily_repeating_slots)

local item_names = require("HUB:zwave/scripts/resources/item_names")

if cc.daily_repeating_slots > 0 then
    local item = self:call("create_item", 
        inclusion_context, 
        build_tree_context, 
        device_node, 
        item_names.weekly_user_code_intervals)

    item.elements_max_number_2 = cc.daily_repeating_slots

    self:call("add_item", inclusion_context, build_tree_context, device_node, item)
elseif cc.week_day_slots > 0 then
    local item = self:call("create_item", 
        inclusion_context, 
        build_tree_context, 
        device_node, 
        item_names.weekly_user_code_intervals,
        "week_day_per_slot")

    item.elements_max_number_2 = cc.week_day_slots
    item.one_week_day_cost = 1
    item.one_shifted_week_day_cost = 2

    self:call("add_item", inclusion_context, build_tree_context, device_node, item)
end

if cc.year_day_slots > 0 then
    local item = self:call("create_item", 
        inclusion_context, 
        build_tree_context, 
        device_node, 
        item_names.daily_user_code_intervals)

    item.elements_max_number_2 = cc.year_day_slots

    self:call("add_item", inclusion_context, build_tree_context, device_node, item)
end
