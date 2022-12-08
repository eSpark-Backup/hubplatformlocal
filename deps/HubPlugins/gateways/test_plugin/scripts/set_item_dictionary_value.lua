local params = ...
if not params then
    print("Parametrs is empty")
    return
end

local core = require "core"

core.update_item_dictionary_value( params.item_id, params.key, params.value )
