local params = ...
if not params then
    print("Parameters is empty")
    return
end

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local zwave = require("zwave")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local handler_creator = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")
local handlers = {} -- handlers cache for optimization: typically items have one handler
local function get_device_handler_monitoring(node_id)
    node_id = math.floor(node_id)
    if handlers[node_id] then
        return handlers[node_id]
    end

    local handler_name = storage:call("get_table_value", storage:call("get_monitoring_handler_key", node_id), "name")

    if handler_name and handlers[handler_name] then
        return handlers[handler_name]
    end

    local handler
    handler, handler_name = handler_creator(node_id)

    if handler_name then
        handlers[handler_name] = handler
        handlers[node_id] = handler
    end

    return handler
end

local function get_fixed_value(descriptor, value)
    if descriptor.class_id == class_ids.dimmer then
        if descriptor.item_name == item_names.dimmer then
            local helper = hubplugins.weak_require("HUB:zwave/scripts/helpers/multilevel_switch_helper")
            value = helper.value_from_ui(value)
        elseif descriptor.item_name == item_names.switch then
            value = value == false and 0x00 or 0x63
        elseif descriptor.item_name == item_names.dimmer_up then
            value = 0x63
        elseif descriptor.item_name == item_names.dimmer_down then
            value = 0x00
        end
    end

    return value
end

local function ensure_table_fields(t, k, ...)
    if k == nil then
        return t
    end
    t[k] = t[k] or {}
    return ensure_table_fields(t[k], ...)
end

local function for_each_group(t, handler, index)
    if index == 0 then
        handler(t)
    else
        for k, group in pairs(t) do
            for_each_group(group, function(...) handler(k, ...) end, index - 1)
        end
    end
end

local function is_multicast_supported(class_id)
    return class_id == class_ids.switch_binary
        or class_id == class_ids.dimmer
        or class_id == class_ids.basic
        or class_id == class_ids.barrier_operator
        or class_id == class_ids.switch_color
end

-- Collect groups to send mulicast requests
local multicast_groups = {}
for _, item_id in ipairs(params.item_ids) do
    local descriptor = storage:call("find_item_descriptor", item_id)
    if descriptor == nil then
        print("[set_item_value_multicast]: item not found with id: " .. item_id)
    else
        local handler = get_device_handler_monitoring(descriptor.node_id)

        if is_multicast_supported(descriptor.class_id) then
            local multicast_value = get_fixed_value(descriptor, params.value)
            local mode = handler:call("get_multicast_mode")
            local grouped_descriptors = ensure_table_fields(multicast_groups, multicast_value, mode, math.floor(descriptor.class_id), math.floor(descriptor.channel_id))

            table.insert(grouped_descriptors, descriptor)
        else
            -- do singlecasr for cc without multicast supporting
            -- TODO: if some bugs happened, implement setting step-by-step
            handler:call("set_item_value", item_id, descriptor, params.value)
        end
    end
end

for_each_group(
    multicast_groups, 
    function(value, mode, class_id, channel_id, grouped_descriptors)
        if #grouped_descriptors == 1 then
            -- do singlecast for single groups

            local descriptor = grouped_descriptors[1]
            local handler = get_device_handler_monitoring(descriptor.node_id)
            local item_id = storage:call("find_item_id", descriptor.item_name, descriptor.node_id, class_id, channel_id, descriptor.tag)

            handler:call("set_item_value", item_id, descriptor, params.value --[[original value]])
        else
            local set_value_multicast = class_id == class_ids.barrier_operator and zwave.set_token_value_multicast or
                                        class_id == class_ids.switch_color     and zwave.set_rgb_value_multicast or
                                                                                   zwave.set_value_multicast

            local node_ids = {}
            for _, descriptor in ipairs(grouped_descriptors) do
                table.insert(node_ids, math.floor(descriptor.node_id))
            end
            table.sort(node_ids) -- specify order for tests

            set_value_multicast(node_ids, class_id, channel_id, value, mode)

            for _, descriptor in ipairs(grouped_descriptors) do
                local handler = get_device_handler_monitoring(descriptor.node_id)
                local method = handler:get_method("on_multicast_value_set")
                if method then
                    method(handler, descriptor, value)
                end
            end
        end
    end,
    4 )
