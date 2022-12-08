-- 12_513_10
local self, inclusion_context, build_tree_context, device_node = ...

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
local notification_cc = build_tree_context.command_class

local function make_event_known(notification_type, unknown_event_type, known_event_type)
    local _, notification = find_if(notification_cc.notifications, function(notification)
        return notification.type == notification_type and notification.unknown_event_types and
            select(2, find_if(notification.unknown_event_types, function(event)
                return event == unknown_event_type
            end))
    end)
    if notification then
        local event_types = notification.event_types or {}
        table.insert(event_types, known_event_type)
        notification.event_types = event_types
    end
end

-- "product_moved" is only supported since cc v8 so it may appear in unknown_event_types because of version mismatch
-- we need to put it to event_types to enable creation of "tampering_move_alarm" item
if notification_cc.version < 8 then
    make_event_known("home_security", 0x09, "product_moved")
end

self:_super_call("default/113", "process_inclusion_impl", inclusion_context, build_tree_context, device_node)
