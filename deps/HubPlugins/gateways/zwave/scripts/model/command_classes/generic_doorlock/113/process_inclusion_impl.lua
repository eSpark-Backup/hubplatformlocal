local self, inclusion_context, build_tree_context, device_node = ...

local function find_cc(id)
    for _, channel in ipairs(inclusion_context.zwave_node.channels) do
        if channel.id == build_tree_context.channel.id then
            for _, cc in ipairs(channel.classes) do
                if cc.id == id then
                    return cc
                end
            end
            break
        end
    end
end

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local notification_cc = find_cc(class_ids.notification)

if notification_cc and (notification_cc.v1_supported or notification_cc.version == 1) then
    local item_names = require("HUB:zwave/scripts/resources/item_names")
    local item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").access_control

    local function create_item(item_name)
        return self:call("create_item", inclusion_context, build_tree_context, device_node, item_name, item_tag)
    end

    local function add_token_item(item_name, supported_operations)
        local item = create_item(item_name)
        assert(item.enum, "enum field expected for item " .. item_name)

        for _, operation in ipairs(supported_operations) do
            for _, existing_operation in ipairs(item.enum) do
                if operation == existing_operation then
                    operation = nil
                    break
                end
            end

            if operation then
                table.insert(item.enum, operation)
            end
        end

        self:call("add_item", inclusion_context, build_tree_context, device_node, item)
    end

    local has_schedule = find_cc(class_ids.schedule_entry_lock) ~= nil or find_cc(class_ids.schedule) ~= nil

    add_token_item(item_names.lock_operation, self:call("get_supported_lock_operations", has_schedule))
    add_token_item(item_names.user_code_operation, self:call("get_supported_user_code_operations"))

    local user_lock_operation_item = create_item(item_names.user_lock_operation)
    self:call("add_item", inclusion_context, build_tree_context, device_node, user_lock_operation_item)
end

self:_super_call("default/113", "process_inclusion_impl", inclusion_context, build_tree_context, device_node)
