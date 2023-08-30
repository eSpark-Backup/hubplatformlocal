local self, inclusion_context, build_tree_context, device_node = ...

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")

local _, home_security = find_if(build_tree_context.command_class.notifications, 
                              function(notification) return notification.type == "home_security" end)

if home_security then
    local index = find_if(home_security.event_types,
                         function(event_type) return event_type == "intrusion" end) -- in fact, the sensor doesn't support intrusion

    index = index or #home_security.event_types + 1
    home_security.event_types[index] = "product_cover_removed"
end

self:_super_call("default/113", "process_inclusion_impl", inclusion_context, build_tree_context, device_node)
