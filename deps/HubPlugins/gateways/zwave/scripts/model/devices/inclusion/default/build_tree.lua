local self, inclusion_context = ...

local build_tree_context = {
    -- channel
    -- command_class
}

for _, channel in ipairs(inclusion_context.zwave_node.channels) do
    inclusion_context.processed_command_class_ids[channel.id] = {}
    build_tree_context.channel = channel

    local device_tree_model = inclusion_context.device_tree_model

    local current_device_node = self:call("create_channel_device", inclusion_context, channel)

    if current_device_node then
        if channel.id == 0 then
            local dev_data = device_tree_model:call("get_data", current_device_node)

            if hubplugins.device_settings_enabled then
                self:call("create_default_settings", dev_data, inclusion_context.zwave_node)
            end
        end

        if channel.classes then
            for _, cc in ipairs(channel.classes) do
                build_tree_context.command_class = cc

                local cc_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_command_class_handler")(inclusion_context.zwave_node, cc.id)
                if cc_handler then
                    local status, error_msg = xpcall(cc_handler.call, debug.traceback, cc_handler, "process_inclusion", inclusion_context, build_tree_context, current_device_node)
                    if not status then
                        print(("Failed to process cc 0x%X: error: %s"):format(cc.id, error_msg))
                    end
                    cc_handler = nil
                end
                hubplugins.try_collectgarbage()
            end
            hubplugins.try_collectgarbage()
        end

        if not device_tree_model:call("has_children", current_device_node) and
           not next(current_device_node.data.items) and
           not next(current_device_node.data.settings) then
            device_tree_model:call("remove_node", current_device_node)
        end
    end
end
