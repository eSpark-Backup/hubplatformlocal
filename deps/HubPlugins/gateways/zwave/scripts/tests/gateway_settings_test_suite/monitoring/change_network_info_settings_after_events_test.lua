local core = require "core"
local tests = require "tests"
local events_handler = loadfile( "HUB:zwave/scripts/model/gateway/events_handling/events_handling" )

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

fixture.set_zwave_info_field("port", "/dev/stick")

-- emulate plugin starting
loadfile( "HUB:zwave/scripts/startup" )()

fixture.zwave_info = {
    module = "Z-Wave",
    module_firmware_version = "6.06",
    locale = "EU",
    home_id = "321",
    node_id = 2,
    port = "/dev/stick_new",
    roles = {
        sis = false,
        primary = false,
        secondary = true,
        static = false,
        inclusion = false
    }
}

local function test_impl(params)
    core.set_gateway_setting_value = tests.create_mock()
    
    events_handler(params.event)
    
    tests.assert_called(
        core.set_gateway_setting_value,
        "check settings values updated",
        { 
            { "zwave.home_id", "321" },
            { "zwave.locale", "EU" },
            { "zwave.node_id", "2" },
            { "zwave.port", "/dev/stick_new" },
            { "zwave.role", "Suc SIS: NO PRI: NO" },
            { "zwave.version", "6.06" },
        }
    )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                event = {event = "module_reset", status = "finished"},
            },
            description = "test with module_reset finished"
        },
        {
            value = {
                event = {event = "learn_finished"},
            },
            description = "test with learn_finished"
        },
        {
            value = {
                event = {event = "sis_node_updated"},
            },
            description = "test with sis_node_updated"
        }
    }
)
