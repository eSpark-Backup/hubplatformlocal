local params = ...

local core = require "core"

print( "TEST_PLUGIN: Update item value, item_id: " .. params.item_id )
if not params.min or not params.max then
    core.update_item_value( params.item_id, params.value )
else
    core.update_item_value_with_min_max( params.item_id, params.value, params.min, params.max )
end
