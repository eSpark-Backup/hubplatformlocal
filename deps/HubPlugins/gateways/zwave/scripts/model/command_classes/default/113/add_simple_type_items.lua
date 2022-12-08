local self, inclusion_context, build_tree_context, device_node, items, notification_type = ...

local current_dev_node = self:call("add_device_if_needed", 
                                    inclusion_context,  
                                    device_node, 
                                    notification_type)

self:call("add_simple_items", inclusion_context, build_tree_context, current_dev_node, items)
