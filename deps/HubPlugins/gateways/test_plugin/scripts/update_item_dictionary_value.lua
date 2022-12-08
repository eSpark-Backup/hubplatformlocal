local params = ...
if not params then
    print("Parameters are empty")
    return
end

local core = require "core"
print(string.format("TEST_PLUGIN: Update item dictionary value, item_id: %s, key: %s", params.item_id, params.key))

core.update_item_dictionary_value(params.item_id, params.key, params.value)