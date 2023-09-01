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

local function send_float_value_updated(value, scale)
    events_handler( {
        event = "float_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.meter,
        type = "water",
        value = value,
        scale = scale,
        precision = 0,
    } )
end

do -- intersection of volume items. Is this case real?
    local channel_0 = {
        id = 0,
        classes = {
            {
                id = class_ids.meter,
                reset_supported = false,
                type = "water",
                scales = {
                    "pulse_count",
                    "cubic_feet",
                    "cubic_meter",
                    "gallons"
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

    local meter_volume_id = storage:call("find_item_id", item_names.water_meter_volume, 9000, class_ids.meter, 0)
    local meter_pulse_id = storage:call("find_item_id", item_names.water_meter_pulse, 9000, class_ids.meter, 0)

    local unit_converter = hubplugins.object("unit_converter", "HUB:zwave/scripts/utils")

    do -- send pulse count
        send_float_value_updated(20, "pulse_count")

        tests.assert_called(
            core.update_item_value,
            "check pulse_count: 20",
            { { meter_pulse_id, 20 } }
        )
    end

    do -- send cubic_meter
        send_float_value_updated(10, "cubic_meter")

        tests.assert_called(
            core.update_item_value,
            "check pulse_count: 10",
            { { meter_volume_id, { value = unit_converter:call("convert", 10, "cubic_meter", "cubic_feet"), scale = "cubic_feet" } } }
        )
    end

    do -- send cubic_feet
        send_float_value_updated(10, "cubic_feet")

        tests.assert_called(
            core.update_item_value,
            "check cubic_feet: 10",
            { { meter_volume_id, { value = 10, scale = "cubic_feet" } } }
        )
    end

    do -- send gallons
        send_float_value_updated(10, "gallons")

        tests.assert_called(
            core.update_item_value,
            "check gallons: 10",
            { { meter_volume_id, { value = unit_converter:call("convert", 10, "gallons", "cubic_feet"), scale = "cubic_feet" } } }
        )
    end

    fixture:remove_node(node)
end


local function test_impl(volume_scale)
    local channel_0 = {
        id = 0,
        classes = {
            {
                id = class_ids.meter,
                reset_supported = false,
                type = "water",
                scales = {
                    "pulse_count",
                    volume_scale,
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

    local meter_volume_id = storage:call("find_item_id", item_names.water_meter_volume, 9000, class_ids.meter, 0)
    local meter_pulse_id = storage:call("find_item_id", item_names.water_meter_pulse, 9000, class_ids.meter, 0)

    do -- send volume
        send_float_value_updated(20, volume_scale)

        tests.assert_called(
            core.update_item_value,
            "check " .. volume_scale .. ": 20",
            { { meter_volume_id, { value = 20, scale = volume_scale } } }
        )
    end

    do -- send pulse
        send_float_value_updated(10, "pulse_count")

        tests.assert_called(
            core.update_item_value,
            "check pulse_count: 10",
            { { meter_pulse_id, 10 } }
        )
    end


    fixture:remove_node(node)
end

tests.run_with_params(
    test_impl,
    {
        {
            value = "cubic_meter",
            description = "cubic_meter and pulse_count"
        },
        {
            value = "cubic_feet",
            description = "cubic_feet and pulse_count"
        },
        {
            value = "gallons",
            description = "gallons and pulse_count" 
        }
    }
)
