local self, inclusion_context, build_tree_context, device_node = ...

build_tree_context.command_class.notifications = {
    {
        event_types = {
            "product_cover_removed",
            "motion_detected"
        },
        type = "home_security",
        unknown_event_types = {}
    }
}

self:_super_call("default/113", "process_inclusion_impl", inclusion_context, build_tree_context, device_node)
