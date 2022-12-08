local self, inclusion_context, build_tree_context, device_node, notification = ...
local notification_mappings  = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/" .. notification.type)

hubplugins.try_collectgarbage()

if not notification_mappings then
    print("[default.113.get_notification_type_items] notification mapping is not defined for: " .. notification.type)
    return nil
end

if not notification.event_types then
    return nil
end

local items = {}

for _, event_type in ipairs(notification.event_types) do
    print(("[default.113.get_notification_type_items] processing of event type: %s (channel %d)"):format(event_type, build_tree_context.channel.id))

    local item_names = notification_mappings.events_to_item_names[event_type]

    if item_names then
        for _, item_name in hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/notification_item_names_iterator")(item_names) do
            local item_data = notification_mappings.items_data[item_name]
            local item = items[item_name]

            if not item then
                item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_name, notification_mappings.item_tag)

                if not item then
                    error(string.format("[process_notification_cc]: need define script for item '%s'!!!", item_name))
                end

                if item_data.initial_enum then
                    for _, token in ipairs(item_data.initial_enum) do
                        table.insert(item.enum, token)
                    end
                end

                items[item_name] = item
            end

            if item_data.get_enum_values and item.value_type == "token" then
                local tokens = item_data.get_enum_values(event_type)
                for _, token in ipairs(tokens) do
                    table.insert(item.enum, token)
                end
            end

            if item_data.get_initial_dictionary_value and string.match( item.value_type, "dictionary") then
                local initial_dictionary_value = item_data.get_initial_dictionary_value(event_type)
                if initial_dictionary_value then
                    for key, value in pairs(initial_dictionary_value) do
                        item.value[key] = value
                    end
                end
            end
        end

    elseif event_type ~= "state_idle" then
        print("[default.113.get_notification_type_items] cannot find item name for event " .. event_type)
    end
    hubplugins.try_collectgarbage()
end

return items
