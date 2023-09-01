local self, zwave_node = ...

-- Work around the fact that we don't detect controlled command classes
local function scene_controllers_workaround_action(field_name, cc_id)
    if self.device_info[field_name] then
        for _, channel in ipairs(zwave_node.channels) do
            if channel.id == 0 then
                table.insert(channel.classes, {
                    id = cc_id,
                    version = 1,
                    [field_name] = self.device_info[field_name]
                })
                break
            end
        end
    end
end

scene_controllers_workaround_action("scene_actions", 43)
scene_controllers_workaround_action("simple_av_control_actions", 148)

local basic_cc_data = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_basic_cc_data_by_device_info")(zwave_node)
if basic_cc_data then
    for _, channel in ipairs(zwave_node.channels) do
        if channel.classes then
            for _, cc in ipairs(channel.classes) do
                if basic_cc_data.id == cc.id then
                    print("Fix cc " .. cc.id .. " due to basic node info")

                    if basic_cc_data.sensor_type then
                        cc.sensor_types = cc.sensor_types or {}
                        table.insert(cc.sensor_types, basic_cc_data.sensor_type)
                        print(("Added '%s' sensor type"):format(basic_cc_data.sensor_type))
                    end
                end
            end
        end
    end
end

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
if storage:call("get_table", storage:call("get_monitoring_handler_key", zwave_node.id)) then
    print("waste monitoring_handler_key for node " .. zwave_node.id .. " exists!")
    storage:call("delete_table", storage:call("get_monitoring_handler_key", zwave_node.id))
end

