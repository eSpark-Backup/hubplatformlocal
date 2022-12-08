local handler, item_id, descriptor, key = ...

local core = require("core")
core.remove_item_dictionary_value(item_id, key)
