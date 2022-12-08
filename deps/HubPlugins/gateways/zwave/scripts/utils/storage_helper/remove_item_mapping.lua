local self, item_id = ...

local descriptors = self:call("find_item_descriptors", item_id)
if not descriptors then
    print("[storage_helper.remove_item_mapping] item_id = " .. item_id .. " not found!")
    return
end

for _, descriptor in pairs(descriptors) do
    if self:call("find_item_id", 
                 descriptor.item_name,
                 descriptor.node_id,
                 descriptor.class_id,
                 descriptor.channel_id,
                 descriptor.tag) == item_id then
        local key = self:call("create_item_descriptor_key",
            descriptor.item_name,
            descriptor.node_id,
            descriptor.class_id,
            descriptor.channel_id,
            descriptor.tag
        )
    
        self._storage.delete(key)
    end
end

self._storage.delete(item_id)
