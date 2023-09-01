local self, item_id = ...

local descriptors = self:call("find_item_descriptors", item_id)
if not descriptors then
    print("LUA: [storage_helper.remove_item_mapping] item_id = " .. item_id .. " not found!")
    return
end

print(#descriptors)

for _, descriptor in ipairs(descriptors) do
    local key = self:call("create_item_descriptor_key",descriptor)
    print("Wipe item " .. item_id .." mapping: " .. key)
    self._storage.delete(key)
end

self._storage.delete(item_id)
