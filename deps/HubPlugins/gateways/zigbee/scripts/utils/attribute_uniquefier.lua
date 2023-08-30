local util = {}

local existing_attributes = {}

function util.unique(node_id, endpoint_id, cluster_name, attribute_name)
    local string = node_id .. "_" .. endpoint_id .. "_" .. cluster_name .. "_" .. attribute_name
    local result = not existing_attributes[string]
    existing_attributes[string] = true
    
    return result
end

function util.unique_by_ids(node_id, endpoint_id, cluster_id, attribute_id)
    return util.unique(
        node_id, 
        endpoint_id, 
        tostring(math.floor(cluster_id)), 
        tostring(math.floor(attribute_id))
    )
end

return util
