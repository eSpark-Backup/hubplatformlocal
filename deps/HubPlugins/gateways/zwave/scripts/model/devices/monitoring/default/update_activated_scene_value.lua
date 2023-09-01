local self, node_id, channel_id, class_id, scene_id = ...

local device_info = self:call("get_device_info")

if device_info.scene_actions then
    for _, scene_info in ipairs(device_info.scene_actions) do
        if scene_id == scene_info.scene_id then
            self:call("handle_scene_activated", node_id, channel_id, class_id, scene_info.button_number, scene_info.button_state)
            break
        end
    end
else
    print(("[handler.update_activated_scene_value]: scene actions are not described for the device. node_id: %g, channel_id: %g"):format(node_id, channel_id))
end
