local node, class_id = ...

for _, channel in ipairs(node.channels) do
    for _, class in ipairs(channel.classes) do
        if class.id == class_id then
            return true
        end
    end
end

return false
