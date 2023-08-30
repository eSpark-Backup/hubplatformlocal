local self, item_id, descriptors = ...

local key_to_descriptor = { }
local unique_descriptors = { }

for _, descriptor in ipairs(descriptors) do
    local key = self:call("create_item_descriptor_key",
        descriptor.item_name,
        descriptor.node_id,
        descriptor.class_id,
        descriptor.channel_id,
        descriptor.tag
    )
    if not key_to_descriptor[key] then
        key_to_descriptor[key] = descriptor
        self._storage.set_string(key, item_id)
        table.insert(unique_descriptors, descriptor)
    end
end

self._storage.set_table(item_id, unique_descriptors);
