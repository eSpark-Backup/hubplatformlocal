local self, inclusion_context = ...

local jaccard_index = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/jaccard_index")

local channels = inclusion_context.processed_command_class_ids

for i = 2, #channels do
    local index = jaccard_index(channels[i-1], channels[i])
    print(("[channel_merge.need_merge_channels] ch: %g, ch: %g, jaccard index: %f"):format(i-1, i, index))
    if index < 0.85 then
        return true
    end
end

return false
