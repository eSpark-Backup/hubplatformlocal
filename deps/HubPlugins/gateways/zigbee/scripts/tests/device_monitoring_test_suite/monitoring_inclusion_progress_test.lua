local tests = require( "tests" )
local core = require( "core" )

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

-- check 10
core.send_ui_broadcast = tests.create_mock()
events_handler( {
    event = "include_device_progress",
    value = 10
} )

tests.assert_called( 
    core.send_ui_broadcast, 
    "check if broadcast sent", 
    { { { event = "include_device_progress", value = 10 } } }
)

-- check 50
core.send_ui_broadcast = tests.create_mock()
events_handler( {
    event = "include_device_progress",
    value = 50
} )

tests.assert_called( 
    core.send_ui_broadcast, 
    "check if broadcast sent", 
    { { { event = "include_device_progress", value = 50 } } }
)

-- check 100
core.send_ui_broadcast = tests.create_mock()
events_handler( {
    event = "include_device_progress",
    value = 100
} )

tests.assert_called( 
    core.send_ui_broadcast, 
    "check if broadcast sent", 
    { { { event = "include_device_progress", value = 100 } } }
)
