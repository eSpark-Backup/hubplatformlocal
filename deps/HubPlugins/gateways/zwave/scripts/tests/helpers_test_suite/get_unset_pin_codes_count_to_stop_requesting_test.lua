local tests = require( "tests" )
local core = require( "core" )

local get_unset_pin_codes_count_to_stop_requesting = loadfile("HUB:zwave/scripts/helpers/get_unset_pin_codes_count_to_stop_requesting")

-- test default value
tests.assert_eq(get_unset_pin_codes_count_to_stop_requesting({}), 5)

-- test specific device value
local device_info = { unset_pin_codes_count_to_stop_requesting = 8 }
tests.assert_eq(get_unset_pin_codes_count_to_stop_requesting(device_info), 8)

-- test no gatewaay setting
core.get_gateway_setting = function(setting_id) end
tests.assert_eq(get_unset_pin_codes_count_to_stop_requesting({}), 5)

-- test gateway setting
core.get_gateway_setting = function(setting_id)
    if setting_id == "zwave.unset_pin_codes_count_to_stop_requesting" then
        return {value = 10}
    end
end

tests.assert_eq(get_unset_pin_codes_count_to_stop_requesting({}), 10)

-- test device specific value is high priority
tests.assert_eq(get_unset_pin_codes_count_to_stop_requesting(device_info), 8)
