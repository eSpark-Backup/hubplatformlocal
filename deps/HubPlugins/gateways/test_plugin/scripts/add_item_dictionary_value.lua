local params = ...
if not params then
    print("Parameters are empty")
    return
end

local core = require "core"

local dict_id = core.generate_item_dictionary_number_id( params.item_id, 1, 1000 )
core.update_item_dictionary_value( params.item_id, dict_id, params.value )
