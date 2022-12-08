local self, inclusion_context = ...


local tree_model = inclusion_context.device_tree_model

self:call("simplify_tree", inclusion_context)

-- 
local new_root = tree_model:call("get_child", tree_model:call("get_root_node"), "siren")

tree_model:call("merge_node_to_parent",
    new_root,
    function(device_data, parent_data)
       for _, item in ipairs(device_data.items) do
            table.insert(parent_data.items, item)
        end
    end
)
