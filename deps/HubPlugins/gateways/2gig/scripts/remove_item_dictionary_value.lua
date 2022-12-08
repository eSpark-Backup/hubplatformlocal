local params = ...

local core = assert(require("core"), "Failed to load core functionality")

loadfile("HUB:2gig/scripts/model/monitoring/remove_item_dictionary_value")(
    params.item_id, 
    params.key
)
