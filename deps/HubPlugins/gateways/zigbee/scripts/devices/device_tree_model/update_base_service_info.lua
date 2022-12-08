local self, new_info = ...

for k, v in pairs(new_info) do
    self._base_device_info[k] = v
end
