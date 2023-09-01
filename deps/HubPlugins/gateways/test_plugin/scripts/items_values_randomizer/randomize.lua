local item_names, item_ids, timeout, cyclicality = ...

local filtered_items_ids = loadfile( "HUB:test_plugin/scripts/items_values_randomizer/collect_items" )( 
    item_names, 
    item_ids
)

if #filtered_items_ids == 0 then
    print( "No items found that respective to filters and value types" )
    return
end

if not timeout then
    for _, item_id in ipairs( filtered_items_ids ) do
        loadfile( "HUB:test_plugin/scripts/items_values_randomizer/randomize_item_value" )( item_id )
    end
    return
end

-- init randomizing by timer
local storage = require "storage"

local last_timer_index = storage.get_number( "last_timer_id" )
last_timer_index = last_timer_index == nil and 1 or last_timer_index + 1
storage.set_number( "last_timer_id", last_timer_index )

local timer_id = "TEST_PLUGIN_randomize_timer_id_" .. tostring( math.floor( last_timer_index ) )

storage.set_number( timer_id, 1 )

local timer_params = {
    item_ids = filtered_items_ids,
    timer_id = timer_id,
    should_repeat = cyclicality == "periodic"
}

local timer = require "timer"

timer.set_interval_with_id( 
    timeout, 
    timer_id, 
    "HUB:test_plugin/scripts/items_values_randomizer/randomize_timer_handler", 
    timer_params 
)
