local device_data, parent_data = ...

for _, item in ipairs(device_data.items) do
    table.insert(parent_data.items, item)
end

-- Note: the following logic assumes that only channel 0 can have setting producing classes (i.e. configuration).
-- So, when we merge into a node with settings, we assume that the other node doesn't have those.
-- The same logic applies when we merge it into another node.

if #parent_data.settings == 0 then
    parent_data.settings = device_data.settings
else
    print("Device \"" .. parent_data.info.name .. "\" will lose some settings!")
end

if parent_data.settings_count == nil then
    parent_data.settings_count = device_data.settings_count
else
    assert(device_data.settings_count == nil)
end
