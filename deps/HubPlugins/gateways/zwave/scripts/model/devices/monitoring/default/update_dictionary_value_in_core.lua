local self, item_name, item_id, dictionary_item_values = ...

local core = require("core")

for key, val in pairs(dictionary_item_values) do
    core.update_item_dictionary_value(item_id, key, val)
end
