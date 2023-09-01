local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )
local zwave = require( "zwave" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local core = require( "core" )

zwave.request_node_values = tests.create_mock()
core.calculate_time_with_delay = function(...) return "2022-01-01T01:01:01" end


local channel_0 = { 
    id = 0,
    classes = { 
        { id = class_ids.thermostat_mode,
            modes = { 0,1,2,7,13 }
        },
        { id = class_ids.thermostat_setpoint,
            setpoints = {
                {type = 1},
                {type = 2},
                {type = 9},
                {type = 15}
            }
        }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local thermostat_energy_saving_mode_item_id = storage:call(
    "find_item_id", 
    item_names.thermostat_energy_saving_mode, 
    9000, 
    class_ids.thermostat_setpoint, 
    0
)
assert(thermostat_energy_saving_mode_item_id)

local function check_updated(mode)
    core.update_item_value = tests.create_mock()
    
    set_item_value({ item_id = thermostat_energy_saving_mode_item_id, value = mode })
    
    tests.assert_called(
        core.update_item_value, 
        "check energy saving mode updated", 
        { { thermostat_energy_saving_mode_item_id, mode } }
    )
end

check_updated("low")
check_updated("off")
check_updated("moderate")
check_updated("aggressive")
check_updated("low_postponed")
check_updated("moderate_postponed")
check_updated("aggressive_postponed")
