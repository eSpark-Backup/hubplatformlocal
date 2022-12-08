local self, inclusion_context = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

for _, command_classes in pairs(inclusion_context.processed_command_class_ids) do
    if command_classes[class_ids.door_lock] then
        return true
    end
end

return false
