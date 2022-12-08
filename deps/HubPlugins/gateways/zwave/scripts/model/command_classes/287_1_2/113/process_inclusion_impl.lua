local self, inclusion_context, build_tree_context, device_node = ...

local cc = build_tree_context.command_class

-- Version 2 of COMMAND_CLASS_ALARM (0x71) does not support the EVENT_SUPPORTED_GET command.
-- Because of that, ZWave node does not contain the list of supported events, so we have to specify them manually.
cc.notifications = {
    {
        type = "home_security",
        event_types = {
            "product_cover_removed"
        },
        unknown_event_types = {}
    }
}

self:_super_call("default/113", "process_inclusion_impl", inclusion_context, build_tree_context, device_node)
