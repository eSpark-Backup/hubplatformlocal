local core = require("core")
local events_handler = loadfile( "HUB:zwave/scripts/model/gateway/events_handling/events_handling" )

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

core.add_gateway_setting = nil
core.remove_gateway_setting = nil
core.set_gateway_setting_value = nil
core.set_gateway_setting_status = nil
core.get_gateway_setting_ids_by_gateway_id = nil
core.get_gateway_setting = nil

-- emulate plugin starting
loadfile( "HUB:zwave/scripts/startup" )()

local function emulate_event_received(event_name, parameters)
    local event = { event = event_name }
    if parameters then
        for key, value in pairs(parameters) do
            event[key] = value
        end
    end
    
    events_handler(event)
end

emulate_event_received( "module_reset", { status = "invoked" } )
emulate_event_received( "module_reset", { status = "started" } )
emulate_event_received( "module_reset", { status = "failed" } )
emulate_event_received( "module_reset", { status = "timeout" } )
emulate_event_received( "module_reset", { status = "finished" } )

emulate_event_received( "reconstruct_started" )
emulate_event_received( "reconstruct_finished" )
emulate_event_received( "reconstruct_finished_error" )

emulate_event_received( "learn_finished" )
