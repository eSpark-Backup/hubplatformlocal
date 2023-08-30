local device_data, parent_data = ...

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/nodes_merger")(device_data, parent_data)

if device_data.info then
    if next(parent_data.info) then
        for key, value in pairs(device_data.info) do
            parent_data.info[key] = value
        end
    else
        parent_data.info = device_data.info
    end
end
