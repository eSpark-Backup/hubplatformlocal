local params = ...

local core = require "core"

if params.item_ids then
    for _, item_id in ipairs(params.item_ids) do
        print( "TEST_PLUGIN: Set item value, item_id: " .. item_id )
        core.update_item_value( item_id, params.value )
    end
else
    print( "TEST_PLUGIN: Set item value, item_id: " .. params.item_id )
    core.update_item_value( params.item_id, params.value )
end
