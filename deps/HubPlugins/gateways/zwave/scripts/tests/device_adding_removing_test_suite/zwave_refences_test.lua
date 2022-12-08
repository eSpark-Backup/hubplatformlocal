local tests = require("tests")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local zwave = require("zwave")
local core = require("core")
zwave.request_node_values = tests.create_mock()

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
        },
        {
            id = 44,
            version = 1
        },
        {
            id = 43,
            version = 1
        }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 139,
    product_id = 21570,
    product_type = 21586,
    is_battery_powered = true,

    specific_type = "SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2",
    generic_type = "GENERIC_TYPE_THERMOSTAT",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}


local expected_references = {
    ["thermostat_mode"] = {
        {
            channel = 0,
            class = 64,
            managing_commands = { "THERMOSTAT_MODE_SET" }
        }
    },
    ["thermostat_setpoint_heating"] = {
        {
            channel = 0,
            class = 67,
            managing_commands = { "THERMOSTAT_SETPOINT_SET" }
        }
    },
    ["thermostat_energy_saving_mode"] = {
        {
            channel = 0,
            class = 67,
            managing_commands = { "THERMOSTAT_SETPOINT_SET" }
        }
    },
    ["thermostat_setpoint_cooling"] = {
        {
            channel = 0,
            class = 67,
            managing_commands = { "THERMOSTAT_SETPOINT_SET" }
        }
    },
    ["thermostat_setpoint"] = {
        {
            channel = 0,
            class = 67,
            managing_commands = { "THERMOSTAT_SETPOINT_SET" }
        }
    }
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- check adding result

for _, item in ipairs(core.get_items()) do
    tests.assert_eq(
        item.info["zwave.references"],
        expected_references[item.name],
        "Compare device info field"
    )
end
