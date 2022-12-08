local params = ...

local core = assert(require("core"), "Failed to load core functionality")

core.update_item_value(params.item_id, params.value)
