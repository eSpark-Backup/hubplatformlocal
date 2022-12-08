local core = require("core")
local tests = require( "tests" )
local smoke_co_detector = require("HUB:2gig/scripts/model/devices/smoke_co_detector")
local smoke_alarm_states = require("HUB:2gig/scripts/enum/smoke_alarm_states")
local co_alarm_states = require("HUB:2gig/scripts/enum/co_alarm_states")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local send_event = loadfile("HUB:2gig/scripts/events/events_handling")
local add_device = loadfile("HUB:2gig/scripts/add_device")

core.update_item_value = tests.create_mock()

add_device({
    model = "2GIG-FF-345",
    serial = 123
})

local item_ids = smoke_co_detector.get_item_ids(123)
local co_detecor_ids = smoke_co_detector.get_item_ids(124)

local function check_event(event_params, expected_call_params, serial)
    send_event({
        event = "honeywell_sensor_status_updated",
        serial = serial,
        loop1 = event_params.loop1,
        tamper_detected = event_params.tamper_detected,
        low_battery = event_params.low_battery
    })
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)
end

local function check_smoke_event(event_params, expected_call_params)
    check_event(event_params, expected_call_params, 123)
end

local function check_co_event(event_params, expected_call_params)
    check_event(event_params, expected_call_params, 124)
end

check_smoke_event(
    {
        loop1 = true,
        tamper_detected = false,
        low_battery = false
    },
    {
        { item_ids.smoke_alarm_id, smoke_alarm_states.not_ok },
        { item_ids.security_threat_id, true },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 100 },
    }
)

check_smoke_event(
    {
        loop1 = false,
        tamper_detected = true,
        low_battery = false
    },
    {
        { item_ids.smoke_alarm_id, smoke_alarm_states.ok },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.not_ok },
        { item_ids.battery_id, 100 },
    }
)

check_smoke_event(
    {
        loop1 = false,
        tamper_detected = false,
        low_battery = true
    },
    {
        { item_ids.smoke_alarm_id, smoke_alarm_states.ok },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 15 },
    }
)

check_co_event(
    {
        loop1 = true,
        tamper_detected = false,
        low_battery = false
    },
    {
        { co_detecor_ids.co_alarm_id, co_alarm_states.not_ok },
        { co_detecor_ids.security_threat_id, true },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 100 },
    }
)

check_co_event(
    {
        loop1 = false,
        tamper_detected = true,
        low_battery = false
    },
    {
        { co_detecor_ids.co_alarm_id, co_alarm_states.ok },
        { co_detecor_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.not_ok },
        { item_ids.battery_id, 100 },
    }
)

check_co_event(
    {
        loop1 = false,
        tamper_detected = false,
        low_battery = true
    },
    {
        { co_detecor_ids.co_alarm_id, co_alarm_states.ok },
        { co_detecor_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 15 },
    }
)
