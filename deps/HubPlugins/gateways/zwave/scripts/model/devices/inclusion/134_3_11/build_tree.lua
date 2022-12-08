local self, inclusion_context = ...

self:_super_call("default", "build_tree", inclusion_context)

local meter_cc_id = require("HUB:zwave/scripts/resources/class_ids").meter

local descriptors = {}

inclusion_context.device_tree_model:call("direct_walk", function(node)
    local data = inclusion_context.device_tree_model:call("get_data", node)

    for _, item in ipairs(data.items) do
        local descriptor = item.descriptor
        if descriptor.channel_id > 0 and descriptor.channel_id < 7 and descriptor.class_id == meter_cc_id then
            descriptors[descriptor.item_name .. "_" .. math.floor(descriptor.channel_id)] = descriptor
        end
    end
end)

for _, descriptor in pairs(descriptors) do
    local fixed_channel_id = descriptor.channel_id > 4 and (descriptor.channel_id - 4) or (descriptor.channel_id + 2)
    if descriptors[descriptor.item_name .. "_" .. math.floor(fixed_channel_id)] then
        descriptor.channel_id = fixed_channel_id
    else
        print("Cannot fix channel " .. descriptor.channel_id .. " -> " .. fixed_channel_id .. " mapping for DSC11 ZWUS!!!")
    end
end
