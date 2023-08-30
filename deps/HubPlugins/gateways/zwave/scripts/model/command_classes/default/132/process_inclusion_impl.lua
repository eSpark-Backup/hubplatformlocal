local self, inclusion_context, build_tree_context, device_node = ...

if build_tree_context.channel.id ~= 0 then return end

local cc = build_tree_context.command_class
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local setting_value = cc.wake_up_interval or 5400
local setting = {
    label = {
        text = "WakeUp Interval"
    },
    description = {
        text = "Wake up interval for sleeping device in seconds "
    },
    value_type = "int",
    value = setting_value,
    value_default = setting_value,
    descriptor = {
        node_id = inclusion_context.zwave_node.id,
        class_id = class_ids.wake_up,
        channel_id = build_tree_context.channel.id,
        tag = "wake_up_interval"
    }
}


if cc.min_wake_up_interval and cc.max_wake_up_interval then
    if cc.min_wake_up_interval == 0 and cc.max_wake_up_interval == 0 then
        return
    else
        setting.value_min = cc.min_wake_up_interval
        setting.value_max = cc.max_wake_up_interval
        setting.value_step = cc.step_wake_up_interval
    end
end

local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)
table.insert(device_node_data.settings, setting)
