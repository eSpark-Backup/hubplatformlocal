local params = ...

local storage = require "storage"

local item_ids = params.item_ids
local timer_id = params.timer_id
local should_repeat = params.should_repeat
local item_index = storage.get_number( timer_id )

if not item_ids[item_index] then
    if not should_repeat then
        print( "Stop randomizing timer with ID " .. timer_id )
    
        storage.delete( timer_id )
        local timer = require "timer"
        timer.cancel( timer_id )
        return
    else
        item_index = 1
    end
end

loadfile( "HUB:test_plugin/scripts/items_values_randomizer/randomize_item_value" )( item_ids[item_index] )

item_index = item_index + 1
storage.set_number( timer_id, item_index )
