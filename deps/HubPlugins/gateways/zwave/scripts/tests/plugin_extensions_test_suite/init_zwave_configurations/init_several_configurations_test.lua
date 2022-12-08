local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/plugins_extensions_fixture")()
local tests = require( "tests" )
local zwave = require( "zwave" )

fixture.init()
fixture.add_extension_data(
    "test_extension_1", 
    {
        ["316_2_12"] = {
            {
                name = "PIR Re-Detect Interval Time",
                description = "In the normal mode, after the PIR motion detected, setting the re-detect time. 8 seconds per tick, default tick is 1 (8 seconds).Setting the suitable value to prevent received the trigger signal too frequently. Also can save the battery energy.",
                type = "int",
                value_min = 1,
                value_max = 127,
                configuration = {
                    number = 8,
                    size = 1,
                    value = 1
                }
            },
            {
                name = "PIR Re-Detect Interval Time",
                description = "In the normal mode, after the PIR motion detected, setting the re-detect time. 8 seconds per tick, default tick is 1 (8 seconds).Setting the suitable value to prevent received the trigger signal too frequently. Also can save the battery energy.",
                type = "int",
                value_min = 1,
                value_max = 127,
                configuration = {
                    number = 9,
                    size = 1,
                    value = 4
                }
            }
        }
    }
)

zwave.clear_device_specific = tests.create_mock()
zwave.add_device_specific_configuration = tests.create_mock()

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/extensions/process_extensions")()
tests.assert_called( zwave.clear_device_specific, "check if device specific storage cleared", nil, 1 )
tests.assert_called( 
    zwave.add_device_specific_configuration, 
    "check if device specific configuration added", 
    {
        {"316_2_12", { number = 8, size = 1, value = 1 }},
        {"316_2_12", { number = 9, size = 1, value = 4 }}
    } 
)
