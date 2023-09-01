local self, inclusion_context, build_tree_context, device_node = ...

local cc = build_tree_context.command_class

if cc.notifications == nil then
    print("[default.113.process_inclusion_impl] no supported notifications")
    return
end

for _, notification in ipairs(cc.notifications) do
    repeat
        print(("[default.113.process_inclusion_impl] processing of notification type: %s (channel %d)"):format(notification.type, build_tree_context.channel.id))

        local items = self:call("get_notification_type_items", inclusion_context, build_tree_context, device_node, notification)

        if not items then
            do break end -- goes to next iteration of for
        end

        local items_adder = self:call("get_items_adder", notification.type)
        items_adder(inclusion_context, build_tree_context, device_node, items)

        if notification.unknown_event_types then
            for _, event_type in ipairs(notification.unknown_event_types) do
                print(("[default.113.process_inclusion_impl] unknown event type: 0x%02X"):format(event_type))
            end
        end

        hubplugins.try_collectgarbage()
    until true
end
