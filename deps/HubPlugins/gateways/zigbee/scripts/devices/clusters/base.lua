local base_cluster_handler = {}

function base_cluster_handler.create_item(self, inclusion_context, attribute_name, item_name, item_descriptor_tag)
    local item_factory = hubplugins.weak_require("HUB:zigbee/scripts/helpers/item_factory")

    local device_node_data = inclusion_context.tree_model:call("get_data", inclusion_context.current_processing_node)
    local item = item_factory.create(item_name, device_node_data.info.device_type_id)

    item.descriptor = {
        item_name = item_name,
        node_id = inclusion_context.zigbee_node.id,
        endpoint_id = inclusion_context.endpoint_id,
        cluster_name = inclusion_context.cluster.name,
        attribute_name = attribute_name,
        tag = item_descriptor_tag,
    }

    return item
end

function base_cluster_handler.create_setting(self, inclusion_context, attribute_name, setting_name)
    local setting_factory = hubplugins.weak_require("HUB:zigbee/scripts/helpers/setting_factory")

    local device_node_data = inclusion_context.tree_model:call("get_data", inclusion_context.current_processing_node)
    local setting = setting_factory.create(setting_name, device_node_data.info.device_type_id)

    setting.descriptor = {
        setting_name = setting_name,
        node_id = inclusion_context.zigbee_node.id,
        endpoint_id = inclusion_context.endpoint_id,
        cluster_name = inclusion_context.cluster.name,
        attribute_name = attribute_name,
    }

    return setting
end

function base_cluster_handler.create_mfg_sprcific_item(self, inclusion_context, attribute_id, item_name, item_descriptor_tag)
    local item_factory = hubplugins.weak_require("HUB:zigbee/scripts/helpers/item_factory")

    local device_node_data = inclusion_context.tree_model:call("get_data", inclusion_context.current_processing_node)
    local item = item_factory.create(item_name, device_node_data.info.device_type_id)

    local mfg_specific_tag_helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/mfg_specific_tag_helper")()
    item.descriptor = {
        item_name = item_name,
        node_id = inclusion_context.zigbee_node.id,
        endpoint_id = inclusion_context.endpoint_id,
        cluster_name = math.floor(inclusion_context.cluster.id),
        attribute_name = math.floor(attribute_id),
        tag = mfg_specific_tag_helper.add_mfg_specific_tag(item_descriptor_tag),
    }

    return item
end

function base_cluster_handler.add_items_and_settings(self, inclusion_context, items, settings, current_dev_node)
    local device_node_data = inclusion_context.tree_model:call(
        "get_data",
        current_dev_node and current_dev_node or inclusion_context.current_processing_node
    )

    for _,item in ipairs(items) do
        if item.value_type == "token" and device_node_data.info.supported_item_states then
            for _, states in ipairs(device_node_data.info.supported_item_states) do
                if states.item_name == item.descriptor.item_name
                and states.endpoint_id == item.descriptor.endpoint_id
                and states.cluster_name == item.descriptor.cluster_name then
                    item.enum = states.enum
                end
            end
        end

        table.insert(device_node_data.items, item)
    end

    if settings then
        for _, setting in ipairs(settings) do
            table.insert(device_node_data.settings, setting)
        end
    end
end

function base_cluster_handler.add_items_and_settings_to_child_device(self, inclusion_context, main_item_name, items, settings)
    local child_device_info = self:find_child_device_info(main_item_name)

    local current_dev_node = inclusion_context.tree_model:call(
        "insert_child",
        inclusion_context.current_processing_node,
        main_item_name,
        child_device_info)

    current_dev_node.data.channel_id = inclusion_context.current_processing_node.data.channel_id

    self:add_items_and_settings(inclusion_context, items, settings, current_dev_node)
end

function base_cluster_handler.find_child_device_info(self, item_name)
    return assert(
        hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/child_device_info/" .. item_name)(),
        "No child device info for item: " .. item_name )
end

function base_cluster_handler.find_attribute(self, attr_name, cluster)
    if not cluster.attributes then return end

    for _, attribute in ipairs(cluster.attributes) do
        if attribute.name == attr_name then
            return attribute
        end
    end
end

function base_cluster_handler.find_command(self, command_name, cluster)
    if not cluster.commands then return end

    for _, command in ipairs(cluster.commands) do
        if command.name == command_name then
            return command
        end
    end
end

function base_cluster_handler.find_attribute_by_id(self, attr_id, cluster)
    if not cluster.attributes then return end

    for _, attribute in ipairs(cluster.attributes) do
        if attribute.id == attr_id then
            return attribute
        end
    end
end

function base_cluster_handler.process_cluster_mfg_specific(self, inclusion_context)
    print("No mfg specific attributes supported")
end

return base_cluster_handler
