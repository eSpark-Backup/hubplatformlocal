local params = ...
if not params then
    print("Parameters are empty")
    return
end

local core = require "core"

core.remove_item_dictionary_value( params.item_id, params.key )
