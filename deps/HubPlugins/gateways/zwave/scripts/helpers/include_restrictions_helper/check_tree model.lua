local self, device_tree_model, node_id = ...

self:call("storage/remove_restricted_node_id", node_id)

local restrictions = self:call("storage/find_include_restrictions")
if not restrictions then return true end
local categories_table, subcategories_table = self:call("_make_restrictions_check_tables", restrictions)

local is_ok = true
local callback = function(dev_node)
    if not is_ok then return end

    local dev_data = device_tree_model:call("get_data", dev_node)

    local node_category = dev_data.info.category
    local node_subcategory = dev_data.info.subcategory

    local is_category_restricted = categories_table[node_category]
    local is_subcategory_restricted = node_subcategory and subcategories_table[node_category] and subcategories_table[node_category][node_subcategory]

    if is_category_restricted or is_subcategory_restricted then
        is_ok = false
    end
end

device_tree_model:call("direct_walk", callback)

if not is_ok then
    self._core.send_ui_broadcast({ event = "restricted_device_not_added" })
    self:call("storage/insert_restricted_node_id", tonumber(node_id))
end

return is_ok
