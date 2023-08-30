local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local core = require("core")

zwave.request_node_values = tests.create_mock()

core.update_item_value = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = {
        {
            id = class_ids.meter,
            reset_supported = false,
            type = "heating",
            scales = {
                "kilo_watt_hour"
            }
        },
    }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

fixture:add_node(node)

local function send_float_value_updated(value, scale)
    events_handler( {
        event = "float_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.meter,
        type = "heating",
        value = value,
        scale = scale,
        precision = 0,
    } )
end

local heating_meter_kwh_id = storage:call("find_item_id", item_names.heating_meter_energy, 9000, class_ids.meter, 0)

do -- send kWh
    send_float_value_updated(30, "kilo_watt_hour")

    tests.assert_called(
        core.update_item_value,
        "check kWh: 30",
        { { heating_meter_kwh_id, { value = 30, scale = "kilo_watt_hour" } } }
    )
end

fixture:remove_node(node)
