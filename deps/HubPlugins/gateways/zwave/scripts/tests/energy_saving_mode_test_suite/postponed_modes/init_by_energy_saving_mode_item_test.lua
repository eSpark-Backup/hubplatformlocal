local tests = require( "tests" )
local core = require( "core" )

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

core.get_local_time = function(...) return "2021-01-01T04:00:00" end
core.get_next_week_event = function(...) return "2022-01-01T04:00:00" end

local function test_impl(mode, with_time)
    storage:call("remove_postponed_state_end_time", fixture.energy_saving_mode_item_id)

    core.calculate_time_with_delay = function(...) return with_time and "2021-01-01T06:00:00" or nil end
    
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({item_id = fixture.energy_saving_mode_item_id, value = mode})
    
    tests.assert_eq(
        storage:call("find_postponed_state_end_time", fixture.energy_saving_mode_item_id), 
        with_time and {year = 2021, mounth = 1, day = 1, hour = 6, minute = 0, second = 0} or nil
    )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = "low_postponed",
            description = "low_postponed mode test"
        },
        {
            value = "moderate_postponed",
            description = "moderate_postponed mode test"
        },
        {
            value = "aggressive_postponed",
            description = "aggressive_postponed mode test"
        }
    },
    {
        {
            value = true,
            description = "test with added delay"
        },
        {
            value = false,
            description = "test with no delay"
        }
    }
)
