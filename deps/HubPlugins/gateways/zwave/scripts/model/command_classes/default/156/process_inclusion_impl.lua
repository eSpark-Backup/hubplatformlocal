local self, inclusion_context, build_tree_context, device_node = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local alarm_sensor_map = hubplugins.weak_require("HUB:zwave/scripts/resources/alarm_sensor_map")
local zwave = require( "zwave" )
local cc_alarm_silence = zwave.get_command_class(inclusion_context.zwave_node.id, build_tree_context.channel.id, class_ids.alarm_silence)

-- main

for _, sensor_type in ipairs(build_tree_context.command_class.sensor_types) do
    local item_name = alarm_sensor_map.sensor_type_to_item_name[sensor_type]
    local item_tag = alarm_sensor_map.sensor_type_to_item_tag[sensor_type]

    if item_name == nil then
        print("alarm sensor inclusion: Cannot map sensor type '" .. sensor_type .. "'' to item name")
    end

    local item = item_name and self:call("get_sensor_item", 
                                        inclusion_context, 
                                        build_tree_context, 
                                        device_node, 
                                        item_name, 
                                        item_tag)

    if item ~= nil then
        local current_dev_node = self:call("add_device_if_needed", inclusion_context, device_node, item.name)
        self:call("add_item", inclusion_context, build_tree_context, current_dev_node, item)

        if cc_alarm_silence ~= nil then
            local sounding_mode_item = self:call("get_sounding_mode_item", 
                                                inclusion_context, 
                                                build_tree_context, 
                                                device_node,
                                                item_tag)

            if sounding_mode_item ~= nil then
                self:call("add_item", inclusion_context, build_tree_context, current_dev_node, sounding_mode_item)
            end
        end
    end

    hubplugins.try_collectgarbage()
end
