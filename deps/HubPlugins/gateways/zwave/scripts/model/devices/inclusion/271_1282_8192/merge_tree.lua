local self, inclusion_context = ...

self:_super_call("default", "merge_tree", inclusion_context)

local tree_model = inclusion_context.device_tree_model
local root = tree_model:call("get_root_node")

local function merge_function(device_data, parent_data)
    for _, item in ipairs(device_data.items) do
        table.insert(parent_data.items, item)
    end
end

tree_model:call("merge_node_to_parent",
    tree_model:call("get_child", root, "temp"),
    merge_function
)

tree_model:call("merge_node_to_parent",
    tree_model:call("get_child", root, 7),
    merge_function
)

local buttons_child = tree_model:call("insert_child", root, "buttons_id", {
        category = "scene_controller",
        subcategory = "",
        name = "Scene Controller",
        type = "scene_controller"
    })

local root_data = tree_model:call("get_data", root)
local root_items = root_data.items
root_data.items = { }
local scene_controller_data = tree_model:call("get_data", buttons_child)
scene_controller_data.items = {  }

local switch_data = tree_model:call("get_data", tree_model:call("get_child", root, 5))
local intrusion_data = tree_model:call("get_data", tree_model:call("get_child", root, 1))
local voltage_data_ch3 = tree_model:call("get_data", tree_model:call("get_child", root, 3))

local item_names = require("HUB:zwave/scripts/resources/item_names")

for _, item in ipairs(root_items) do
    if item.name == item_names.button_state then
        table.insert(scene_controller_data.items, item)
    elseif item.name == item_names.switch then
        table.insert(switch_data.items, item)
    elseif item.name == item_names.intrusion_alarm then
        table.insert(intrusion_data.items, item)
    elseif item.name == item_names.electric_meter_volt then
        table.insert(voltage_data_ch3.items, item)
    else
        table.insert(root_data.items, item)
    end
end

tree_model:call("merge_node_to_sibling", root, item_names.humidity, 9, merge_function)
