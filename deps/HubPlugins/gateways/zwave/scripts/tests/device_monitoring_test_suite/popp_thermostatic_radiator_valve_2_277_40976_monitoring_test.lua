local zwave = require("zwave")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local core = require("core")

local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

zwave.request_node_values = tests.create_mock()
zwave.set_setpoint_value = tests.create_mock()

local function update_setpoint_value(value, scale)
    events_handler({
        event = "thermostat_setpoint_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        type = 1,
        value = value,
        scale = scale
    })
end

local channel_0 = {
    classes = {
        {
            id = 32,
            is_supported = false,
            version = 0
        },
        {
            id = 112,
            parameters = {},
            version = 0
        },
        {
            id = 128,
            version = 1
        },
        {
            id = 129,
            version = 1
        },
        {
            id = 114,
            version = 1
        },
        {
            id = 49,
            sensors = {
                {
                    scales = {
                        "celsius"
                    },
                    sensor_type = "air_temperature"
                }
            },
            version = 6
        },
        {
            id = 67,
            setpoints = {
                {type = 1}
            },
            version = 2
        },
        {
            firmware_versions = {
                "1.0"
            },
            hardware_version = 0,
            id = 134,

            library_type = "routing_slave",
            version = 1,
            zwave_protocol_version = "3.67"
        },
        {
            id = 132,
            version = 2
        }
    },
    id = 0
}

local node = {
    id = "9000",
    manufacturer_id = 2,
    product_type = 277,
    product_id = 40976,
    is_battery_powered = true,

    generic_type = "GENERIC_TYPE_THERMOSTAT",
    specific_type = "SPECIFIC_TYPE_UNKNOWN",

    security = {"none"},
    security_class_started = {"none"},
    wake_up_interval = 1800,
    channels = {channel_0}
}

-- prepare zwave
zwave.add_node(node)

-- add node
events_handler({
    event = "node_added",
    node_id = 9000
})

local setpoint_heating_item_id = storage:call("find_item_id", item_names.thermostat_setpoint_heating, "9000",
                                              class_ids.thermostat_setpoint, 0)

update_setpoint_value(21, 0)
local heating_item = core.get_item(setpoint_heating_item_id)
tests.assert_eq(heating_item.value, { value = 21, scale = "celsius" })

update_setpoint_value(68, 1)
heating_item = core.get_item(setpoint_heating_item_id)
tests.assert_eq(heating_item.value, { value = 68, scale = "fahrenheit" })
