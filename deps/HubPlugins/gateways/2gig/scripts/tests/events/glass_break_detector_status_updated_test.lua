local core = require("core")
local tests = require( "tests" )
local glass_break_detector = require("HUB:2gig/scripts/model/devices/glass_break_detector")
local glass_breakage_states = require("HUB:2gig/scripts/enum/glass_breakage_states")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local send_event = loadfile("HUB:2gig/scripts/events/events_handling")
local add_device = loadfile("HUB:2gig/scripts/add_device")

core.update_item_value = tests.create_mock()

add_device({
    model = "2GIG-GB1-345",
    serial = 123
})

local item_ids = glass_break_detector.get_item_ids(123)

local function check_event(event_params, expected_call_params)
    send_event({
        event = "glass_break_detector_status_updated",
        serial = 123,
        glass_break_detected = event_params.glass_break_detected,
        tamper_detected = event_params.tamper_detected,
        low_battery = event_params.low_battery
    })
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)

    send_event({
        event = "honeywell_sensor_status_updated",
        serial = 123,
        loop1 = event_params.glass_break_detected,
        tamper_detected = event_params.tamper_detected,
        low_battery = event_params.low_battery
    })
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)
end

check_event(
    {
        glass_break_detected = true,
        tamper_detected = false,
        low_battery = false
    },
    {
        { item_ids.glass_breakage_alarm_id, glass_breakage_states.not_ok },
        { item_ids.security_threat_id, true },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 100 },
    }
)

check_event(
    {
        glass_break_detected = false,
        tamper_detected = true,
        low_battery = false
    },
    {
        { item_ids.glass_breakage_alarm_id, glass_breakage_states.ok },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.not_ok },
        { item_ids.battery_id, 100 },
    }
)

check_event(
    {
        glass_break_detected = false,
        tamper_detected = false,
        low_battery = true
    },
    {
        { item_ids.glass_breakage_alarm_id, glass_breakage_states.ok },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 15 },
    }
)
