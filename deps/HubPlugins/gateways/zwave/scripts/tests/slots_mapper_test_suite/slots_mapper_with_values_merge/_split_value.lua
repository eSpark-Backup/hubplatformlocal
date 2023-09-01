local self, value, sub_value_handler = ...

for _, sub_value in ipairs(value) do
    sub_value_handler(sub_value)
end

