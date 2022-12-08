local self, inclusion_context = ...

local tree_model = inclusion_context.device_tree_model

local root_device = tree_model:call("get_root_node")
local dev_channel_0 = tree_model:call("get_child", root_device, 0)

local counter = 1
local dev_channel = tree_model:call("get_child", root_device, counter)
while dev_channel ~= nil do
    self:call("merge_nodes", inclusion_context, dev_channel, dev_channel_0)

    counter = counter + 1
    dev_channel = tree_model:call("get_child", root_device, counter)
end
