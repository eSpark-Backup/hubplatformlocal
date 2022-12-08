local node, endpoint_id, cluster_name, attribute_name = ...

for _,endpoint in ipairs(node.endpoints) do
    if endpoint.id == endpoint_id then
        for _,cluster in ipairs(endpoint.clusters) do
            if cluster.name == cluster_name then
                for _,attribute in ipairs(cluster.attributes) do
                    if attribute.name == attribute_name then return attribute end
                end
            end
        end
    end
end
