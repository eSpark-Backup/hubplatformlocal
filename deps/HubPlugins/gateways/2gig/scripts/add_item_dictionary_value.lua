local params = ...

local core = assert(require("core"), "Failed to load core functionality")

local key = core.generate_item_dictionary_number_id(params.item_id, 1, 100)
if key then
    loadfile("HUB:2gig/scripts/model/monitoring/set_item_dictionary_value")(
        params.item_id, 
        tostring(key), 
        params.value
    )
else
    print("Unable to add new dictionary value, container is full")
end
