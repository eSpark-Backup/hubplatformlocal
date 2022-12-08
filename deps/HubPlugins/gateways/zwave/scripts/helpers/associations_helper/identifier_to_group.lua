local self, identifier = ...

local channel_id, group_id = string.match(identifier, "(%d+)_(%d+)")
if channel_id and group_id then
    return tonumber(channel_id), tonumber(group_id)
end
