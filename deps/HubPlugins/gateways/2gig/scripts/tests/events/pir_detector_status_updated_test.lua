local core = require("core")
local tests = require( "tests" )
local pir_detector = require("HUB:2gig/scripts/model/devices/pir_detector")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local send_event = loadfile("HUB:2gig/scripts/events/events_handling")
local add_device = loadfile("HUB:2gig/scripts/add_device")

core.update_item_value = tests.create_mock()

add_device({
    model = "2GIG-PIR1-345",
    serial = 123
})

local item_ids = pir_detector.get_item_ids(123)

local function check_event(event_params, expected_call_params)
    send_event({
        event = "pir_detector_status_updated",
        serial = 123,
        motion_detected = event_params.motion_detected,
        tamper_detected = event_params.tamper_detected,
        low_battery = event_params.low_battery
    })
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)

    send_event({
        event = "honeywell_sensor_status_updated",
        serial = 123,
        loop1 = event_params.motion_detected,
        tamper_detected = event_params.tamper_detected,
        low_battery = event_params.low_battery
    })
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)
end

check_event(
    {
        motion_detected = true,
        tamper_detected = false,
        low_battery = false
    },
    {
        { item_ids.motion_id, true },
        { item_ids.security_threat_id, true },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 100 },
    }
)

check_event(
    {
        motion_detected = false,
        tamper_detected = true,
        low_battery = false
    },
    {
        { item_ids.motion_id, false },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.not_ok },
        { item_ids.battery_id, 100 },
    }
)

check_event(
    {
        motion_detected = false,
        tamper_detected = false,
        low_battery = true
    },
    {
        { item_ids.motion_id, false },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 15 },
    }
)
