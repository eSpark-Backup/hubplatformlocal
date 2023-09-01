local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/fibaro_smart_implant_fgbs222_fixture")()
local zwave = require("zwave")
local tests = require( "tests" )

zwave.request_node_values = tests.create_mock()

local function test_impl(sensors_count)

    local node = fixture:create_ds18s20_node(sensors_count)

    fixture:add_node(node)

    local expected_devices = fixture:create_base_expected_devices()

    for i = 1, sensors_count do
        table.insert(expected_devices[1].subdevices, {
                name = sensors_count == 1 and "Temperature Sensor" or tests.skipped_param,
                category = "temperature",
                items = {
                    {
                        name = item_names.temp,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 7 + i,
                    },
                    {
                        name = "sw_state",
                        cc_id = class_ids.notification,
                        tag = "system",
                        channel_id = 7 + i
                    },
                }
            })
    end

    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {},
        tests = tests
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
    hubplugins.weak_loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

    fixture:remove_node(node)

    hubplugins.weak_loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
end

tests.run_with_params(test_impl, {
    {
        value = 1,
        description = "One DS18S20 connected sensor"
    },
    {
        value = 2,
        description = "2 DS18S20 connected sensors"
    },
    {
        value = 6,
        description = "6 DS18S20 connected sensors"
    },
})
