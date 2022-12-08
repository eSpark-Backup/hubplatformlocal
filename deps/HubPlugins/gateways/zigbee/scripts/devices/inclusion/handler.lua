local dev_info = ...

local handler = { dev_info = dev_info }

local core = assert(require "core", "Fail to load core functionality")
local zigbee = assert(require "zigbee", "Fail to load zigbee functionality")
local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local security_items = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/security_items")()


function handler.process_node( self, node )
    print( "LUA: function handler.process_node" )

    local inclusion_context = self:build_new_tree_model(node)

    self:add_tree_to_core(inclusion_context)

    self:configure_attributes_reporting(inclusion_context.tree_model)

    self:send_inclusion_finished_broadcast()

    -- request all items values
    hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/request_device_values")(storage:call("find_device_id", node.id))
end

function handler.build_new_tree_model( self, node )
    local tree_model = self:create_device_tree(node)

    local inclusion_context = {
        tree_model = tree_model,
        zigbee_node = node,
        current_processing_node = tree_model:call("get_root_node")
    }

    self:build_tree(inclusion_context)
    self:merge_tree(inclusion_context)

    hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/validate_device_info")({ tree_model = tree_model, zigbee_node = node })

    return inclusion_context
end


function handler.create_device_tree( self, node )
    local devInfo = self.dev_info
    devInfo.gateway_id = core.get_gateway().id
    print( "LUA: devInfo.gateway_id = " .. devInfo.gateway_id)
    devInfo.battery_powered = node.is_battery_powered
    dev_info.device_type_id = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/create_device_type_id")(node.manufacturer_id, node.model_id)

    local device_tree_model = hubplugins.object("device_tree_model", "HUB:zigbee/scripts/devices", dev_info)
    device_tree_model:call("insert_child") -- insert root node

    return device_tree_model
end


function handler.build_tree(self, inclusion_context)
    local node = inclusion_context.zigbee_node
    local tree_model = inclusion_context.tree_model
    for _, endpoint in ipairs( node.endpoints ) do
        local current_device_node = tree_model:call("insert_child", tree_model:call("get_root_node"), endpoint.id)
        current_device_node.data.endpoint_id = endpoint.id

        inclusion_context.endpoint_id = endpoint.id
        inclusion_context.current_processing_node = current_device_node

        for _, cls in ipairs( endpoint.clusters ) do
            inclusion_context.cluster = cls

            self:process_cluster(inclusion_context)
            self:process_cluster_mfg_specific_attributes(inclusion_context)
        end
    end
end


function handler.process_cluster( self, inclusion_context )
    print( "LUA: function handler.process_cluster for nodeId " ..
        tostring(inclusion_context.zigbee_node.id) ..
        ", endpoint_id " ..
        tostring(inclusion_context.endpoint_id) ..
        ", cluster_name " ..
        inclusion_context.cluster.name)

    local device_node_data = inclusion_context.tree_model:call("get_data", inclusion_context.current_processing_node)

    local cls_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_cluster_handler")(
        device_node_data.info.device_type_id,
        inclusion_context.cluster.name
    )

    if not cls_handler then
        print("LUA: no cluster handler found for cluster name: " .. inclusion_context.cluster.name)
        return
    end

    cls_handler:process_cluster(inclusion_context)
end

function handler.process_cluster_mfg_specific_attributes( self, inclusion_context )
    local device_node_data = inclusion_context.tree_model:call("get_data", inclusion_context.current_processing_node)

    local cluster_identifier = inclusion_context.cluster.name == "unknown" and
        math.floor(inclusion_context.cluster.id) or
        inclusion_context.cluster.name

    local cls_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_cluster_handler")(
        device_node_data.info.device_type_id,
        cluster_identifier
    )

    if not cls_handler then
        print("LUA: no mfg specific cluster handler found for cluster: " .. cluster_identifier)
        return
    end

    cls_handler:process_cluster_mfg_specific(inclusion_context)
end

function handler.merge_tree(self, inclusion_context)
    local device_tree_model = inclusion_context.tree_model

    self:simplify_endpoints(device_tree_model)
    self:detect_new_root(device_tree_model)
end

function handler.simplify_endpoints(self, device_tree_model)
    -- merge enpoints subtrees independantly
    local nodes_to_merge = {}
    device_tree_model:call(
        "for_each_child",
        device_tree_model:call("get_root_node"),
        function(endpoint_node)
            local is_self_sufficient = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/is_self_sufficient_device")(device_tree_model:call("get_data", endpoint_node))
            if is_self_sufficient then return end
            local new_endpoint

            device_tree_model:call(
                "for_each_child",
                endpoint_node,
                function(endpoint_child)
                    if not new_endpoint then
                        new_endpoint = endpoint_child
                        return
                    end

                    local first_data = device_tree_model:call("get_data", new_endpoint).info
                    local second_data = device_tree_model:call("get_data", endpoint_child).info
                    if self:is_priority_device_less(first_data, second_data) then
                        new_endpoint = endpoint_child
                    end
                end)

            if new_endpoint ~= endpoint_node then
                table.insert(nodes_to_merge, new_endpoint)
            end
        end)

    for _, node_to_merge in ipairs(nodes_to_merge) do
        device_tree_model:call(
            "merge_node_to_parent",
            node_to_merge,
            function(device_data, parent_data)
                parent_data.info = device_data.info

                for _, item in ipairs(device_data.items) do
                    table.insert(parent_data.items, item)
                end

                for _, setting in ipairs(device_data.settings) do
                    table.insert(parent_data.settings, setting)
                end
            end)
    end

    -- delete empty endpoints
    local nodes_to_delete = {}
    device_tree_model:call(
        "for_each_child",
        device_tree_model:call("get_root_node"),
        function(endpoint_node)
            local dev_data = device_tree_model:call("get_data", endpoint_node)
            if #dev_data.items == 0 then
                table.insert(nodes_to_delete, endpoint_node)
            end
        end)

    for _, node_to_delete in ipairs(nodes_to_delete) do
        device_tree_model:call("remove_node", node_to_delete)
    end

end

function handler.is_priority_device_less(self, lhs, rhs)
    local child_device_priority = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/child_device_priority")()

    local lhs_category = child_device_priority[lhs.category]
    local rhs_category = child_device_priority[rhs.category]

    if lhs_category.priority > rhs_category.priority then
        return true
    elseif lhs_category.priority == rhs_category.priority then
        local lhs_subcategory = lhs_category.subcategory[lhs.subcategory]
        local rhs_subcategory = rhs_category.subcategory[rhs.subcategory]

        return lhs_subcategory and rhs_category and (lhs_subcategory > rhs_subcategory)
    end

    return false
end

function handler.detect_new_root(self, device_tree_model)
    local new_root = nil
    device_tree_model:call(
        "for_each_child",
        device_tree_model:call("get_root_node"),
        function(endpoint_node)
            if not new_root then
                new_root = endpoint_node
            elseif endpoint_node.data.endpoint_id < new_root.data.endpoint_id then
                new_root = endpoint_node
            end
        end)

    if new_root then
        device_tree_model:call(
            "merge_node_to_parent",
            new_root,
            function(device_data, parent_data)
                parent_data.info = device_data.info

                for _, item in ipairs(device_data.items) do
                    table.insert(parent_data.items, item)
                end

                for _, setting in ipairs(device_data.settings) do
                    table.insert(parent_data.settings, setting)
                end
            end)
     end
end


function handler.add_tree_to_core(self, inclusion_context)
    local device_tree_model = inclusion_context.tree_model
    local zigbee_node_id = inclusion_context.zigbee_node.id

    local callback = function(dev_node)
        local core = assert(require("core"), "Fail to load the 'core' functionality")

        local dev_data = device_tree_model:call("get_data", dev_node)
        local parent_dev_node = device_tree_model:call("get_parent", dev_node)

        if not parent_dev_node then
            dev_data.info.id = assert(core.add_device(dev_data.info), "Adding device to 'core' module failed")
            storage:call("insert_device_mapping", dev_data.info.id, zigbee_node_id)
        else
            local parent_dev_data = device_tree_model:call("get_data", parent_dev_node)
            dev_data.info.parent_device_id = parent_dev_data.info.id
            dev_data.info.id = core.add_device(dev_data.info)
        end

        local merged_items_map = self:get_merged_items(dev_data.items)

        local dev_data_info_id = dev_data.info.id
        for _, merged_item_info in ipairs(merged_items_map) do
            merged_item_info.main_item.device_id = dev_data_info_id
            local item_id = core.add_item(merged_item_info.main_item)
            storage:call("insert_item_mapping", item_id, merged_item_info.descriptors)
        end

        for _, setting in ipairs(dev_data.settings) do
            setting.device_id = dev_data_info_id
            local setting_id = core.add_setting(setting)
            storage:call("insert_setting_mapping", setting_id, setting.descriptor)
        end
    end

    device_tree_model:call("direct_walk", callback)
end

function handler.configure_attributes_reporting(self, tree_model)
    local callback = function(dev_node)
        local dev_data = tree_model:call("get_data", dev_node)
        hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/configure_attributes_reporting")(dev_data)
    end

    tree_model:call("direct_walk", callback)
end

function handler.send_inclusion_finished_broadcast(self)
    core.send_ui_broadcast({ event = "include_device_progress", value = 100 })
    core.send_ui_broadcast({ event = "include_device", status = "finished" })
end

function handler.get_merged_items(self, items)
    local merged_items_map = { }
    local result_map = { }

    for _, item in ipairs(items) do
         self:add_item_to_merged_map(merged_items_map, result_map, item)
    end

    for _, item_info in pairs(merged_items_map) do
        table.insert(result_map, item_info)
    end

    return result_map
end

function handler.add_item_to_merged_map(self, merged_items_map, result_map, item)
    assert(item.name)

    -- do not merge "button_state" items
    if item.name == item_names.button_state then
        table.insert(result_map, { main_item = item, descriptors = { item.descriptor } })
        return
    end

    local merged_item_info = merged_items_map[item.name]
    if merged_item_info then
        self:merge_items(merged_item_info.main_item, item)
        table.insert(merged_item_info.descriptors, item.descriptor)
    else
        merged_items_map[item.name] = {
            main_item = item,
            descriptors = { item.descriptor }
        }

        if security_items[item.name] then
            local security_item = {
                name = item_names.security_threat,
                value_type = "bool",
                has_getter = true,
                has_setter = false,
                value = false
            }

            local security_tag_helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/security_tag_helper")()
            security_item.descriptor = {
                item_name = item.name,
                node_id = item.descriptor.node_id,
                endpoint_id = item.descriptor.endpoint_id,
                cluster_name = item.descriptor.cluster_name,
                attribute_name = item.descriptor.attribute_name,
                tag = security_tag_helper.add_security_tag(item.descriptor.tag)
            }

            self:add_item_to_merged_map(merged_items_map, result_map, security_item)
        end
    end
end

function handler.merge_items(self, target_item, source_item)
    if target_item.value_type ~= source_item.value_type then
        print(("[base_command_class.merge_items]: value types differs: %s - %s"):format(target_item.value_type, source_item.value_type))
        return
    end

    target_item.has_setter = target_item.has_setter or source_item.has_setter
    target_item.has_getter = target_item.has_getter or source_item.has_getter

    if target_item.value_type == "token" then
        local tokens_set = {}
        for _, token_value in ipairs(target_item.enum) do
            tokens_set[token_value] = true
        end

        for _, token_value in ipairs(source_item.enum) do
            if not tokens_set[token_value] then
                table.insert(target_item.enum, token_value)
            end
        end
    else
        print(("[base_command_class.merge_items]: how to merge items of type '%s' ?"):format(target_item.value_type))
    end
end

return handler
