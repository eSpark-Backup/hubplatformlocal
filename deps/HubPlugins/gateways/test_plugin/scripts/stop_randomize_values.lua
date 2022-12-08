local storage = require "storage"
local timer = require "timer"

local last_timer_index = storage.get_number( "last_timer_id" )

if not last_timer_index then return end

for i = 1, last_timer_index do
    local timer_id = "TEST_PLUGIN_randomize_timer_id_" .. tostring( math.floor( i ) )

    print( "Stop randomizing timer with ID " .. timer_id )
    
    storage.delete( timer_id )
    timer.cancel( timer_id )
end

storage.delete( "last_timer_id" )
