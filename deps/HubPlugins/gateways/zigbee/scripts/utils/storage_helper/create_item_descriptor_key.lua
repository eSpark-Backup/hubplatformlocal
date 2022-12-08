local self, descriptor = ...

local key = 
    descriptor.item_name .. "_".. 
    tostring(math.floor(descriptor.node_id)) .. "_" .. 
    tostring(math.floor(descriptor.endpoint_id)) .. "_" .. 
    tostring(descriptor.cluster_name) .. "_" ..
    tostring(descriptor.attribute_name)

 if descriptor.tag then
     return key .. '_' .. descriptor.tag
 end

return key
