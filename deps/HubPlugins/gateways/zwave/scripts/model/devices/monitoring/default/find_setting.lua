local self, node_id, class_id, tag = ...

local core = assert(require("core"), "Failed to load core functionality")

local setting_id = self:call("find_setting_id", node_id, class_id, tag)

if setting_id ~= nil then
    return core.get_setting(setting_id)
end
