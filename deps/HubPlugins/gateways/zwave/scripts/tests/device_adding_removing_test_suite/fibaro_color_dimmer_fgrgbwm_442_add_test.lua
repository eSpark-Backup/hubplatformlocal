local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local function test_impl( product_info )
    local function make_channel(id)
        return {
            id = id,
            classes = {
                { id = 0x26 } -- This test check for device names, not items created
            }
        }
    end

    local node = {
        id = "9000",
        channels =  {
            make_channel(0),
            make_channel(1),
            make_channel(2),
            make_channel(3),
            make_channel(4),
            make_channel(5),
            make_channel(6),
            make_channel(7),
            make_channel(8),
            make_channel(9),
        },
        generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
        specific_type = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL",
        is_battery_powered = false,
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id,
        security =  { "none" },
        security_class_started =  { "none" }
    }

    fixture:add_node(node)

    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {},
        tests = tests
    }

    local expected_main_items = {
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 0
        },
    --[[    {
            name = "basic",
            cc_id = class_ids.basic,
            channel_id = 0
        },
        {
            name = "hw_state",
            cc_id = class_ids.notification,
            tag = "system",
            channel_id = 0
        },
        {
            name = "voltage_drop_drift_state",
            cc_id = class_ids.notification,
            tag = "power_management",
            channel_id = 0
        },
        {
            name = "load_error_state",
            cc_id = class_ids.notification,
            tag = "power_management",
            channel_id = 0
        },
        {
            name = "power_surge_state",
            cc_id = class_ids.notification,
            tag = "power_management",
            channel_id = 0
        },
        {
            name = "over_current_state",
            cc_id = class_ids.notification,
            tag = "power_management",
            channel_id = 0
        },
        {
            name = "over_load_state",
            tag = "power_management",
            cc_id = class_ids.notification,
            channel_id = 0
        },
        {
            name = "meter_reset",
            cc_id = class_ids.meter,
            channel_id = 0
        },
        {
            name = "electric_meter_kwh",
            cc_id = class_ids.meter,
            channel_id = 0
        },
        {
            name = "electric_meter_watt",
            cc_id = class_ids.multilevel_sensor,
            channel_id = 0,
        },
        {
            name = "electric_meter_watt",
            cc_id = class_ids.meter,
            channel_id = 0,
            is_merged = true,
        },--]]

        -- 1st channel
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 1,
            is_merged = true,
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 1,
            is_merged = true,
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 1,
            is_merged = true,
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 1,
            is_merged = true,
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 1,
            is_merged = true,
        },

        -- 6th channel
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 6,
            is_merged = true,
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 6,
            is_merged = true,
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 6,
            is_merged = true,
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 6,
            is_merged = true,
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 6,
            is_merged = true,
        },

        -- 7th channel
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 7,
            is_merged = true,
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 7,
            is_merged = true,
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 7,
            is_merged = true,
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 7,
            is_merged = true,
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 7,
            is_merged = true,
        },

        -- 8th channel
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 8,
            is_merged = true,
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 8,
            is_merged = true,
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 8,
            is_merged = true,
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 8,
            is_merged = true,
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 8,
            is_merged = true,
        },

        -- 9th channel
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 9,
            is_merged = true,
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 9,
            is_merged = true,
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 9,
            is_merged = true,
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 9,
            is_merged = true,
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 9,
            is_merged = true,
        },
    }

    local expected_red_dimmer_items = {
        -- 2nd channel
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 2,
            is_merged = false,
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 2,
            is_merged = false,
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 2,
            is_merged = false,
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 2,
            is_merged = false,
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 2,
            is_merged = false,
        },

    }

    local expected_green_dimmer_items = {
        -- 3rd channel
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 3,
            is_merged = false,
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 3,
            is_merged = false,
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 3,
            is_merged = false,
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 3,
            is_merged = false,
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 3,
            is_merged = false,
        },
    }

    local expected_blue_dimmer_items = {
        -- 4th channel
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 4,
            is_merged = false,
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 4,
            is_merged = false,
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 4,
            is_merged = false,
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 4,
            is_merged = false,
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 4,
            is_merged = false,
        },
    }

    local expected_white_dimmer_items = {
       -- 5th channel
        {
            name = "dimmer_up",
            cc_id = class_ids.dimmer,
            channel_id = 5,
            is_merged = false,
        },
        {
            name = "dimmer_stop",
            cc_id = class_ids.dimmer,
            channel_id = 5,
            is_merged = false,
        },
        {
            name = "switch",
            cc_id = class_ids.dimmer,
            channel_id = 5,
            is_merged = false,
        },
        {
            name = "dimmer",
            cc_id = class_ids.dimmer,
            channel_id = 5,
            is_merged = false,
        },
        {
            name = "dimmer_down",
            cc_id = class_ids.dimmer,
            channel_id = 5,
            is_merged = false,
        },
    }

    local expected_devices = {
        {
            name = "Fibaro RGBW Controller",
            category = "dimmable_light",
            subcategory = "dimmable_colored",
            items = expected_main_items,
            node_id = "9000",
            battery_powered = false,
            security = "none",
            subdevices = {
                {
                    name = "Red",
                    category = "dimmable_light",
                    subcategory = "dimmable_colored",
                    items = expected_red_dimmer_items,
                },
                {
                    name = "Green",
                    category = "dimmable_light",
                    subcategory = "dimmable_colored",
                    items = expected_green_dimmer_items,
                },
                {
                    name = "Blue",
                    category = "dimmable_light",
                    subcategory = "dimmable_colored",
                    items = expected_blue_dimmer_items,
                },
                {
                    name = "White",
                    category = "dimmable_light",
                    subcategory = "dimmable_colored",
                    items = expected_white_dimmer_items,
                },
            }
        }
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
    loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

    fixture:remove_node(node)

    loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)

end

tests.run_with_params( test_impl,
{
    {
        value = {
            manufacturer_id = 271,
            product_type = 2306,
            product_id = 4096,
        },
        description = "FIBARO RGBW Controller 2 FGRGBW-442, Multicolor 271_2306_4096"
    },
    {
        value = {
            manufacturer_id = 271,
            product_type = 2306,
            product_id = 8192,
        },
        description = "FIBARO RGBW Controller 2 FGRGBW-442, Multicolor 271_2306_8192"
    },
} )