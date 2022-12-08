local self, new_info = ...

local function update_table(target, source)
    for k, v in pairs(source) do
        if type(target[k]) == "table" then
            update_table(target[k], v)
        else
            target[k] = v
        end
    end
end

update_table(self._base_device_info, new_info)
