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
            type = "electric",
            scales = {
                "kilo_watt_hour",
                "kilo_volt_ampere_hour",
                "watt",
                "pulse_count",
                "volt",
                "ampere",
                "power_factor",
                "kilo_volt_ampere_reactive",
                "kilo_volt_ampere_reactive_hour",
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
        type = "electric",
        value = value,
        scale = scale,
        precision = 0,
    } )
end

local electric_meter_kwh_id = storage:call("find_item_id", item_names.electric_meter_kwh, 9000, class_ids.meter, 0)
local electric_meter_kvah_id = storage:call("find_item_id", item_names.electric_meter_kvah, 9000, class_ids.meter, 0)
local electric_meter_watt_id = storage:call("find_item_id", item_names.electric_meter_watt, 9000, class_ids.meter, 0)
local electric_meter_pulse_id = storage:call("find_item_id", item_names.electric_meter_pulse, 9000, class_ids.meter, 0)
local electric_meter_volt_id = storage:call("find_item_id", item_names.electric_meter_volt, 9000, class_ids.meter, 0)
local electric_meter_amper_id = storage:call("find_item_id", item_names.electric_meter_amper, 9000, class_ids.meter, 0)
local electric_meter_factor_id = storage:call("find_item_id", item_names.electric_meter_factor, 9000, class_ids.meter, 0)
local electric_meter_kvar_id = storage:call("find_item_id", item_names.electric_meter_kvar, 9000, class_ids.meter, 0)
local electric_meter_kvarh_id = storage:call("find_item_id", item_names.electric_meter_kvarh, 9000, class_ids.meter, 0)

do -- send kWh
    send_float_value_updated(10, "kilo_watt_hour")

    tests.assert_called(
        core.update_item_value,
        "check kWh: 10",
        { { electric_meter_kwh_id, { value = 10, scale = "kilo_watt_hour" } } }
    )
end

do -- send kVAh
    send_float_value_updated(10, "kilo_volt_ampere_hour")

    tests.assert_called(
        core.update_item_value,
        "check kVAh: 10",
        { { electric_meter_kvah_id, { value = 10, scale = "kilo_volt_ampere_hour" } } }
    )
end

do -- send W
    send_float_value_updated(10, "watt")

    tests.assert_called(
        core.update_item_value,
        "check W: 10",
        { { electric_meter_watt_id, { value = 10, scale = "watt" } } }
    )
end

do -- send pulse count
    send_float_value_updated(10, "pulse_count")

    tests.assert_called(
        core.update_item_value,
        "check pulse count: 10",
        { { electric_meter_pulse_id, 10 } }
    )
end

do -- send V
    send_float_value_updated(10, "volt")

    tests.assert_called(
        core.update_item_value,
        "check V: 10",
        { { electric_meter_volt_id, { value = 10, scale = "volt" } } }
    )
end

do -- send A
    send_float_value_updated(10, "ampere")

    tests.assert_called(
        core.update_item_value,
        "check A: 10",
        { { electric_meter_amper_id, { value = 10, scale = "ampere" } } }
    )
end

do -- send power factor
    send_float_value_updated(10, "power_factor")

    tests.assert_called(
        core.update_item_value,
        "check power factor: 10",
        { { electric_meter_factor_id, { value = 10, scale = "float" } } }
    )
end

do -- send kVar
    send_float_value_updated(10, "kilo_volt_ampere_reactive")

    tests.assert_called(
        core.update_item_value,
        "check kVar: 10",
        { { electric_meter_kvar_id, { value = 10, scale = "kilo_volt_amper_reactive" } } }
    )
end

do -- send kVarh
    send_float_value_updated(10, "kilo_volt_ampere_reactive_hour")

    tests.assert_called(
        core.update_item_value,
        "check kVarh: 10",
        { { electric_meter_kvarh_id, { value = 10, scale = "kilo_volt_amper_reactive_hour" } } }
    )
end

fixture:remove_node(node)

