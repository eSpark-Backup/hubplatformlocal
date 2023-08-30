local self, zwave_node_id = ...

return {
    label = {
        text = "Zwave Association Groups"
    },
    description = {
        text = "Represents list of supported zwave association groups"
    },
    value_type = "dictionary.association_group",
    value = {},
    status = "synced",
    descriptor = {
        node_id = zwave_node_id,
        class_id = nil,
        tag = "associations_groups"
    },
    has_setter = false
}
