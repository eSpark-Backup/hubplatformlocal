local self, item_id, item_descriptors = ...

local key_to_descriptor = { }
local unique_descriptors = { }

for _, descriptor in ipairs(item_descriptors) do
    local key = self:call("create_item_descriptor_key", descriptor)
    if not key_to_descriptor[key] then
        key_to_descriptor[key] = descriptor
        self._storage.set_string(key, item_id)
        table.insert(unique_descriptors, descriptor)
    end
end

self._storage.set_table(item_id, unique_descriptors);
